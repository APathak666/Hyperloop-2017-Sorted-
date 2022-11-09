/*
 *  1) Header Files
 */

// a. required for FreeRTOS
#include <kinetis.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
// b. required for IMU
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/*
 *  2) Global Variables and Macros
 */

/*
 *  2.a) Pressure Sensor Global Variables
 */
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno1 = Adafruit_BNO055(55,0x28);
Adafruit_BNO055 bno2 = Adafruit_BNO055(55,0x29);

unsigned long old_time = 0;
unsigned long new_time = 0;

unsigned long acceleration1[3];
unsigned long acceleration2[3];

unsigned long euler1[3];
unsigned long euler2[3];

unsigned long velocity1[3];
unsigned long displacement1[3];

unsigned long velocity2[3];
unsigned long displacement2[3];

  imu::Vector<3> acc1;
  imu::Vector<3> acc2;
  imu::Vector<3> eul1;
  imu::Vector<3> eul2;

/*
 *  2.b) Retro Reflective Sensor Global Variables
 */
// pin number
const byte interrupt_pin=4;

// all variables changing in isr must be declared volatile
volatile int strip_count=0;// no of strips passed
volatile int isr_call=0; // number of times isr is called
volatile long timer_begin=0; // timer at beginning of the strip
volatile long timer_end=0;// timer at the end of the strip

double time_width;
double velocity;
double strip_width=0.1016; // 4 inch strip

int pos[]={0,1200,2400,3600,4800,6000,7200,8400,9600,10800,12000,12008,12016,12024,12032,12040,12048,12054,12064,12072,13200,14400,15600,16800,18000,18008,18016,18024,18032,19200,20400,21600,22800};// array with position of strips corresponding to strip_count

/*
 *  2.c) Pressure Sensor Global Variables
 */
uint16_t output;
byte address_pressure=0x28;
float pressure;

/*
 *  2.d) Temperature Sensor Global Variables
 */
  uint16_t temp;
  float tempmag;
  byte address_temp = 0x18;

/*
 *  3. Function Prototypes
 */

/*
 *  3.a) FreeRTOS Tasks
 *       (external tasks declared elsewhere in program)
 */
// sensor tasks
static void IMU(void* args);
static void RetroReflective(void* args);
static void Temp(void* args);
static void Pressure(void* args);
// task to periodically broadcast data over CAN
static void CANsend(void* args);

/*
 *  3.b) Other Functions
 */
// log data
void logData(void* args);
// utility function for retro ref sensor
void retro_isr1();

/*
 * 4) MAIN
 */
int main() {
  /*
   * 4.a) Create Tasks
   */
  xTaskCreate(IMU, "IMU", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(RetroReflective, "RR", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(Temp, "TS", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(Pressure, "PS", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(CANsend, "CAN", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

  /*
   * 4.b) IMU initialization
   */
  Serial.begin(9600);
  Serial.println("Orientation Sensor Raw Data Test"); Serial.println("");
  if(!bno1.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  if(!bno2.begin())
  {
    /*There was a problem detecting the BNO055 ... check your connection */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  //delay(1000);
  bno1.setExtCrystalUse(true);
  bno2.setExtCrystalUse(true);

  Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");
  /*
   * 4.c) initialization for RetroReflective
   */
  //Serial.begin(9600);
  pinMode(interrupt_pin,INPUT);
  attachInterrupt(digitalPinToInterrupt(interrupt_pin),retro_isr1,CHANGE);
  /*
   * 4.d) initialization for Temperature and Pressure
   */
  Wire1.begin();
  Wire1.setClock(400000L);
  return 0;
  /*
   * 4.e) start scheduler, main should stop functioning here
   */
  vTaskStartScheduler();

  for (;;); // main should never reach to this point, unless there are some errors
}

/*
 * 5) Tasks
 */

/*
 * 5.a) IMU
 */
void IMU(void * args)
{
    //(void) args;
    const TickType_t xDelay100ms = pdMS_TO_TICKS( BNO055_SAMPLERATE_DELAY_MS );
    // 1.read
    while(1)
    {
      new_time=micros();

      // Possible vector values can be:
      // - VECTOR_ACCELEROMETER - m/s^2
      // - VECTOR_MAGNETOMETER  - uT
      // - VECTOR_GYROSCOPE     - rad/s
      // - VECTOR_EULER         - degrees
      // - VECTOR_LINEARACCEL   - m/s^2
      // - VECTOR_GRAVITY       - m/s^2

      acc1 = bno1.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
      acc2 = bno2.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
      eul1 = bno1.getVector(Adafruit_BNO055::VECTOR_EULER);
      eul2 = bno2.getVector(Adafruit_BNO055::VECTOR_EULER);

      /* Display the floating point data */
      /* Serial.print("X: ");
      Serial.print(acc1.x());
      Serial.print(" Y: ");
      Serial.print(acc1.y());
      Serial.print(" Z: ");
      Serial.print(acc1.z());
      Serial.print("\t\t");*/

      euler1[0]=eul1.x()*PI/180;
      euler1[1]=eul1.y()*PI/180;
      euler1[2]=eul1.z()*PI/180;
      euler2[0]=eul2.x()*PI/180;
      euler2[1]=eul2.y()*PI/180;
      euler2[2]=eul2.z()*PI/180;

      acceleration1[0]=acc1.x()*cos(euler1[2])*cos(euler1[1])+acc1.y()*sin(euler1[2])*sin(euler1[1])-acc1.z()*sin(euler1[1]);
      acceleration2[0]=acc2.x()*cos(euler2[2])*cos(euler2[1])+acc2.y()*sin(euler2[2])*sin(euler2[1])-acc2.z()*sin(euler2[1]);


      //displacement1[0]=velocity1[0]*(new_time-old_time)+0.5*acceleration1[0]*(new_time-old_time)*(new_time-old_time);
      //displacement[1]=velocity[1]*(new_time-old_time)+0.5*acc1.y()*(new_time-old_time)*(new_time-old_time);
      //displacement[2]=velocity[2]*(new_time-old_time)+0.5*acc1.z()*(new_time-old_time)*(new_time-old_time);

      //displacement2[0]=velocity2[0]*(new_time-old_time)+0.5*acceleration2[0]*(new_time-old_time)*(new_time-old_time);

      velocity1[0]=velocity1[0]+acceleration1[0]*(new_time-old_time);
      velocity2[0]=velocity2[0]+acceleration2[0]*(new_time-old_time);
      //velocity[1]=velocity[1]+acc1.y()*(new_time-old_time);
      //velocity[2]=velocity[2]+acc1.z()*(new_time-old_time);

      displacement1[0]+=velocity1[0]*(new_time-old_time);
      displacement2[0]+=velocity2[0]*(new_time-old_time);

      Serial.print("Acc: ");
      Serial.print((acc1.x()+acc2.x())/2);
      Serial.print("vel: ");
      Serial.print((velocity1[0]+velocity2[0])/2+"\t");
      Serial.print("displacement: ");
      Serial.print((displacement1[0]+displacement2[0])/2+"\t\n");
      /*
      // Quaternion data
      imu::Quaternion quat = bno.getQuat();
      Serial.print("qW: ");
      Serial.print(quat.w(), 4);
      Serial.print(" qX: ");
      Serial.print(quat.y(), 4);
      Serial.print(" qY: ");
      Serial.print(quat.x(), 4);
      Serial.print(" qZ: ");
      Serial.print(quat.z(), 4);
      Serial.print("\t\t");
      */

      /* Display calibration status for each sensor. */
     /* uint8_t system, gyro, accel, mag = 0;
      bno.getCalibration(&system, &gyro, &accel, &mag);
      Serial.print("CALIBRATION: Sys=");
      Serial.print(system, DEC);
      Serial.print(" Gyro=");
      Serial.print(gyro, DEC);
      Serial.print(" Accel=");
      Serial.print(accel, DEC);
      Serial.print(" Mag=");
      Serial.println(mag, DEC);*/

      old_time=new_time;

      vTaskDelay( xDelay100ms );
    }
    // 2. log data
    //logData();
}

/*
 * 5.b) RetroReflective
 */
void RetroReflective(void * args)
{
  for(;;)
  {
    // put your main code here, to run repeatedly:
    if(isr_call%2==0 && isr_call!=0)
    {
      strip_count++;                  // count of strips crossed
      time_width=timer_end-timer_begin;// time difference for crossing a strip
      time_width=time_width/1000000;// convert micro seconds to seconds
      velocity=strip_width/time_width;// velocity in m/s
      isr_call=0;// or else this block keeps executing
    }
    else
    {
      timer_begin=timer_end;
    }
  }
    // 2. log data
    //logData();
}
/*
 * 5.c) Temperature
 */
void Temp(void * args)
{
  const TickType_t xDelay250ms = pdMS_TO_TICKS( 250 );
  for (;;)
  {
    // put your main code here, to run repeatedly:
    Wire.beginTransmission(address_temp|0x00);
    Wire.write(0x05);
    Wire.endTransmission();
    Wire.requestFrom(address_temp|0x00,2);// if you put Wire.beginTra... then you'll have to start again
    while(Wire.available())
    {
      temp=Wire.read();
      temp = temp<<8;
      temp=temp|Wire.read();
    }

    tempmag=temp&0x0fff;
    tempmag /=16;

    if(temp&0x1000)
    {
      tempmag=256-tempmag;
    }

    vTaskDelay( xDelay250ms );//tConv for 0.0625
  }
}
/*
 * 5.d) Pressure
 */
void Pressure(void * args)
{
  (void) args;
  for(;;)
  {
    Wire1.requestFrom(address_pressure,(byte)2);
    while(Wire1.available())
    {
      output=Wire1.read();
      output=output<<8;
      output=output|Wire1.read();
    }
    output=output&0x0fff;
    pressure=((output/0x3fff)-0.1)/0.8;
    Serial.println(pressure);
  }
}
/*
 * 5.e) CANsend for broadcast over CAN
 */
void CANsend(void * args) {
  (void) args;
  for (;;) {
  }
}
/*
 * 6) Other Functions
 */

/*
 * 6.a) Log Data to SD Card
 */
void logData() {
  ;
}
/*
 * 6.b) utility Function for RetroReflective
 */
void retro_isr1(){
  isr_call++; // number of times isr is called
  timer_end=micros();// time when isr is called
}
