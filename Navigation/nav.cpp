// 1. Edit global to local assignment of position, velo and acc in RRTasks
// Send Low Speed drive packet
#include <kinetis.h>
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <FlexCAN.h>
#include <queue.h>
#include <timers.h>
#include <usb_dev.h>
#include <SD.h>
#include <SPI.h>
#include <stdlib.h>
#include <semphr.h>
#include <event_groups.h>
#include <IntervalTimer.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <i2c_t3.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>

#include <cmath>

#define Display Serial1
#define Serial_BAUD 9600
#define CAN_BAUD  1000000
#define Acc_Pin 14
#define rr1_pin 14  // 35  //Front retro
#define rr2_pin 16  // 36  //Back retro 
#define ACC_DELAY 100
#define LTS1_PIN A18
#define LTS2_PIN A20
#define LTS3_PIN A21
#define LTS4_PIN A22
#define DataLogTimerPeriod 1000  
#define DEBUG_MODE 1
#define TEST_MODE 1
#define PUSHER_DETACHMENT_POSITION 487.68
#define POD_MASS 300

//System State
#define LVElec            0
#define Idle              1
#define Ready             2
#define AwaitPusherAttach 3
#define Pushing          4
#define LevAndBraking    5
#define LowSpeedDrive     6
#define PodStop          7
#define EmStop           8
#define Fault            9
#define EB               10 
#define PowerOff         11

volatile int system_state=0;

#define AccMin -20
#define AccMax 20
#define YawMin -0.5
#define YawMax 0.5
#define PitchMin -1.3
#define PitchMax  1.3
#define ACC_ERROR 1

#define  XGyroOffset 177
#define  YGyroOffset -33
#define  ZGyroOffset -52
#define  ZAccelOffset 870
#define  YAccelOffset -1525
#define  XAccelOffset -108 // 1688 factory default for my test chip

#define HIGH_V 3.2  
#define LOW_V 0.64
#define HIGH_D 8.5
#define LOW_D 3.5

#define Kp 10
#define Ki 0
#define Kd 0.5

#define STATECHNGID 0x21
#define SYSSTATEACK 0x41
#define ACCGYRO     0x51
#define POSVELO     0x52
#define LTSB12      0x53
#define LTSB34      0x54
#define RRSCOUNT    0x55
#define BRAKING     0x61
#define LSDRPM      0x62
#define LINACT      0x63
#define EMGNCY      0xE0
#define CAN_ACK_FAILURE_PERIOD 100

#define TWO_STRIPE_DISTANCE 30.48
#define TWO_STRIPE_DISTANCE_WITH_TOLERANCE 45.72
#define TWO_SENSOR_DISTANCE_WITH_TOLERANCE 1.7 //1.6 to be exact

#define LOW_SPEED_POSITION_THRESHOLD 1234.76

TaskHandle_t TASK1=NULL, TASK2=NULL, TASK3=NULL, TASK4=NULL, TASK5=NULL, TASK6=NULL, TASK7=NULL;              //Declare Task Handles
TaskHandle_t LinAct=NULL, BPID=NULL, brRetraction=NULL, BrStop=NULL, E=NULL, CANS=NULL, ACCEL=NULL, BrComp=NULL, LSD=NULL;

//--------------------------------------------------------------------------------------------------

volatile int counter1 = 0;
volatile int counter2 = 0;
volatile byte timer3_status = 0;
volatile float t1,t2,t3 =0;

IntervalTimer timer1;
IntervalTimer timer2;
IntervalTimer timer3;

void rr1_isr();
void rr2_isr();
void rr1_missed();
void timer1_isr();
void timer2_isr();
void timer3_isr();

//-------------------------------------------------------------------------------------------------------------------------------
const int chipSelect = BUILTIN_SDCARD;
const float DIS_BW_2_STRIPES=10;
const float DIS_BW_2_SENSORS=10;

bool initialise_status=1;
volatile float global_acceleration1=0;
volatile float global_acceleration2=0;
volatile float global_acceleration=0;
volatile float global_velocity=0;
volatile float global_position=0;
volatile float global_yaw=0;
volatile float global_pitch=0;
volatile float global_roll=0;
volatile uint8_t braking_sep_1=0;
volatile uint8_t braking_sep_2=0;
volatile uint8_t braking_sep_3=0;
volatile uint8_t braking_sep_4=0;
volatile uint8_t braking_sep_avg=0;
volatile uint8_t stripe_count = 0; //for counting the number of strips passed by retroreflective sensor

volatile bool ack_counter1 = false;
volatile bool ack_counter2 = false;
volatile bool ack_counter3 = false;

volatile bool pid_direction = true;
volatile float needed_gap_height = 0;


String dataString="";

MPU6050 mpu;
bool blinkState = false;
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector

uint16_t counter = 0;
volatile bool mpuInterrupt = false;

int sensor1_error_counter=0;
int sensor2_error_counter=0;
int timer3_error_counter=0;

TimerHandle_t dataLogTimer, stateAckFailureTimer;
File dataFile;
SemaphoreHandle_t accMutex, dmpMutex;
SemaphoreHandle_t PKTHDLE[3];
SemaphoreHandle_t attitudeMutex;
SemaphoreHandle_t brakingPIDValueSem;

CAN_message_t sys_state_change;
CAN_message_t sys_state_ack;
CAN_message_t health;
CAN_message_t lts_packet1;
CAN_message_t lts_packet2;
CAN_message_t attitude_acc;
CAN_message_t pos_vel;
CAN_message_t stripe_count_msg;
CAN_message_t LinAct_msg;
CAN_message_t brake_msg;
CAN_message_t lsd_msg;

volatile int flag;
volatile int rr_flag;

// void Task1(void* args);
// void Task2(void* args);
// void Task3(void* args);
// void Task4(void* args);
// void Task5(void* args);
// void Task6(void* args);
// void Task7(void* args);
// void isrService1();
// void isrService2();

CAN_message_t msgRec[2];

//----------------------------CAN code begins here --------------------------------------------------------------------------------------------

class NavCANListener : public CANListener 
{
public:
  bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

NavCANListener canListener1;

bool NavCANListener::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
  int index = (frame.id & 0x1FFFFFF8) >> 5;
  msgRec[index] = frame;
  xSemaphoreGiveFromISR(PKTHDLE[index], pdFALSE);
  return true;
}

//Receiving System State Change Packets
void CANReceive1(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[1], portMAX_DELAY);
    #ifdef DEBUG_MODE
//      Serial1.println("Entering CANReceive1");
    #endif
    taskDISABLE_INTERRUPTS();
    system_state = msgRec[1].buf[0];
    Can0.write(sys_state_ack); 
    taskENABLE_INTERRUPTS();
    #ifdef DEBUG_MODE
//      Serial1.println("Exiting CANReceive1");
    #endif
  }
}

//Receiving System State Change Acks packet
void CANReceive2(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[2], portMAX_DELAY);
    #ifdef DEBUG_MODE
      Serial1.print("New system state : ");
      Serial1.println(msgRec[2].buf[0]);
    #endif
    taskDISABLE_INTERRUPTS();
    if (msgRec[2].id==0x42)
      ack_counter1 = 1;
    if (msgRec[2].id==0x43)
      ack_counter2 = 1;
    if (msgRec[2].id==0x44)
      ack_counter3 = 1;
    taskENABLE_INTERRUPTS();  //Send system state change acknowledge
  }
}

void alivePacket(void *args){
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS( 100 );
  xLastWakeTime = xTaskGetTickCount();  
  
  for(;;){ 
    health.ext= 0;
    health.id = 0x01; //1
    health.len = 1;
    Can0.write(health);
    vTaskDelayUntil( &xLastWakeTime, xPeriod );
  }
}

//xTimer1Started = xTimerStart( xOneShotTimer, 0 );
void canAckFailureCallback( TimerHandle_t xTimer ){
  if(ack_counter1 && (ack_counter2 && ack_counter3)==0){

  }
  else{
    ack_counter1 = 0;
    ack_counter2 = 0;
    ack_counter3 = 0;
  }
}

/*

0. Low Voltage Electronics 
1. Idle
2. Ready
3. Awaiting Pusher Attachment
4. Pushing
5. Levitation & Braking
6. Descent & Retraction Of Brakes
7. Rolling on Wheels
8. Low Speed Drive
9. Pod Stop
10. Emergency Stop
11. Fault         // Failure
12. EB0
13. EB1
14. EB2
15. Power Off
*/

//--------------------------------------CAN code ends and MPU working code starts here-------------------------------------------------------------------

void dmpDataReady() {
  BaseType_t xYieldRequired;
    
  mpuInterrupt = true;
  fifoCount = mpu.getFIFOCount();
  
  if (fifoCount > packetSize) {
    xYieldRequired = xTaskResumeFromISR( ACCEL );
    if( xYieldRequired == pdTRUE ){
      portYIELD_FROM_ISR( ACCEL );
    }
  }
}


void accelerometer(void *args){
    // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif

  // initialize device
  #ifdef DEBUG_MODE
    Serial.println(F("Initializing I2C devices..."));
  #endif
  mpu.initialize();

  // verify connection
  #ifdef DEBUG_MODE
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
    Serial.println(F("Initializing DMP..."));
  #endif

  // load and configure the DMP  
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
      // turn on the DMP, now that it's ready
    #ifdef DEBUG_MODE
      Serial.println(F("Enabling DMP..."));
    #endif
    mpu.setDMPEnabled(true);

    pinMode(23, INPUT);
    attachInterrupt(23, dmpDataReady, FALLING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    #ifdef DEBUG_MODE
      Serial.println(F("DMP ready! Waiting for first interrupt..."));
    #endif
    
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } 

  else {
      #ifdef DEBUG_MODE
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
      #endif
  }

  for(;;){
    while (!dmpReady) {
      Display.println("DMP not ready");
    }
    
    vTaskSuspend( NULL );
      
    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();
    
    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        mpu.resetFIFO();
        #ifdef DEBUG_MODE
          Serial.println(F("FIFO overflow!"));
        #endif

    } else if (mpuIntStatus & 0x02) {// otherwise, check for DMP data ready interrupt (this should happen frequently)
        while (fifoCount < packetSize) 
          fifoCount = mpu.getFIFOCount();
        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);          // track FIFO count here in case there is > 1 packet available (this lets us immediately read more without waiting for an interrupt)        
        fifoCount -= packetSize;
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetAccel(&aa, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
        mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
       
        #ifdef DEBUG_MODE
          Serial.print("aworld\t");
          Serial.print(aaWorld.x);
          Serial.print("\t");
          Serial.print(aaWorld.y);
          Serial.print("\t");
          Serial.print(aaWorld.z);
          Serial.print("\t");
          Serial.println(counter++);
        #endif
    }
  }
}


void poseComputation(void* args)
{
  float local_position=0, local_velocity=0, local_acceleration1=0, local_acceleration2=0, averaged_local_acceleration=0;
  for(;;)
  {
    #ifdef DEBUG_MODE
  //    Serial1.println("Entering Dead Reckoning Task");
      // Serial1.print("Acc");
      // Serial1.println(averaged_local_acceleration);
      // Serial1.print("Velocity");
      // Serial1.println(global_velocity);
      // Serial1.print("Position");
      // Serial1.println(global_position);

    #endif
    
    local_acceleration1 = global_acceleration1;
    local_acceleration2 = global_acceleration2; 

    local_velocity = global_velocity;
    local_position = global_position; 

    if(abs(local_acceleration1 - local_acceleration2) < ACC_ERROR)
    {
      averaged_local_acceleration = (local_acceleration1 + local_acceleration2)/2;
      
      if (averaged_local_acceleration>AccMin && averaged_local_acceleration<AccMax)
      {
        local_velocity += averaged_local_acceleration * ACC_DELAY/1000;
        local_position += local_velocity * ACC_DELAY/1000;  

        global_velocity=local_velocity;
        global_position=local_position; 
        global_acceleration = averaged_local_acceleration;
      
        if (system_state == AwaitPusherAttach && averaged_local_acceleration > 0.1 && local_velocity!=0)
        {          
          taskENTER_CRITICAL();
          sys_state_change.buf[0]= Pushing;
          system_state = Pushing;
          Can0.write(sys_state_change);
          xTaskNotifyGive(LinAct);
          taskEXIT_CRITICAL();
        }

        else if (system_state==LevAndBraking && local_velocity < 20)
        {
          taskENTER_CRITICAL();
          system_state = LowSpeedDrive;
          sys_state_change.buf[0]=LowSpeedDrive;
          Can0.write(sys_state_change);
          vTaskDelete(BPID);
          needed_gap_height = 32;
          pid_direction = -1;
          brake_msg.buf[0] = 3;
          Can0.write(brake_msg);
          xTaskNotifyGive(LSD);
          taskEXIT_CRITICAL();                //Stop braking PID
          //Delete the PID task
        }

        else if (system_state==LowSpeedDrive && (local_velocity == 0 || averaged_local_acceleration < 0.1))
        {    
          taskENTER_CRITICAL();        
          system_state = PodStop;
          sys_state_change.buf[0]=PodStop;
          Can0.write(sys_state_change);
          taskEXIT_CRITICAL(); 
        }
      }
      
      else
      {
        //Declare problem
      }
    }
    else
    {
//      xTaskNotifyGive(E);  // Send notification to Emergency Task
    }  
    
    vTaskDelay(pdMS_TO_TICKS(ACC_DELAY));

    //Check for correct brackets here
  }
}

//---------------------------------------------------IMU ends and LTS begins----------------------------------------------------------------------

void init_lts(){
  pinMode(LTS1_PIN,INPUT);
  pinMode(LTS2_PIN,INPUT);
  pinMode(LTS3_PIN,INPUT);
  pinMode(LTS4_PIN,INPUT);
  analogReadResolution(16);
}

void lts (void* args){
  init_lts();
  float distance1=0;
  float v1 = 0;
  float distance2=0;
  float v2 = 0;
  float distance3=0;
  float v3 = 0;
  float distance4=0;
  float v4 = 0;
  

  for(;;){ 
    v1 = analogRead(LTS1_PIN)*3.3/65535;
    distance1 = (v1-LOW_V)/(HIGH_V - LOW_V) *(HIGH_D - LOW_D) + LOW_D;
    v2 = analogRead(LTS2_PIN)*3.3/65535;
    distance2 = (v2-LOW_V)/(HIGH_V - LOW_V) *(HIGH_D - LOW_D) + LOW_D;
    v3 = analogRead(LTS3_PIN)*3.3/65535;
    distance3 = (v3-LOW_V)/(HIGH_V - LOW_V) *(HIGH_D - LOW_D) + LOW_D;
    v4 = analogRead(LTS4_PIN)*3.3/65535;
    distance4 = (v4-LOW_V)/(HIGH_V - LOW_V) *(HIGH_D - LOW_D) + LOW_D;
    
    #ifdef DEBUG_MODE 
      Serial1.print("LTS Voltage1");
      Serial1.println(v1);
      Serial1.print("LTS Distance1");
      Serial1.println(distance1,4);
      
      Serial1.print("LTS Voltage2");
      Serial1.println(v2);
      Serial1.print("LTS Distance2");
      Serial1.println(distance2,4);

      Serial1.print("LTS Voltage3");
      Serial1.println(v3);
      Serial1.print("LTS Distance3");
      Serial1.println(distance3,4);

      Serial1.print("LTS Voltage4");
      Serial1.println(v4);
      Serial1.print("LTS Distance4");
      Serial1.println(distance4,4);
    #endif
    
    braking_sep_1 = distance1;
    braking_sep_2 = distance2;
    braking_sep_3 = distance3;
    braking_sep_4 = distance4;
    braking_sep_avg = (distance1 + distance2 + distance3 + distance4)/4;

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}


//---------------------------------------Retro Reflective Sensor Code begins && LTS code ends--------------------------------------------------------------


void retro_state_machine(void *args)
{
    pinMode(rr1_pin,INPUT);
    pinMode(rr2_pin,INPUT);
    attachInterrupt(rr1_pin,rr1_isr,RISING);
    attachInterrupt(rr2_pin,rr2_isr,RISING);

    for(;;){      
      if(counter1 == 2 || counter2 == 2){
          #ifdef DEBUG_MODE
            Serial1.println("//CRITICAL");
          #endif
        taskENTER_CRITICAL();    
        system_state = EB;
        sys_state_change.buf[0]=EB;
        Can0.write(sys_state_change);
        taskEXIT_CRITICAL();
      }    
    }
  }

void rr1_isr(){

  //Serial.println("R1");
  //Serial.println(global_position);

  #ifdef DEBUG_MODE
//    Serial1.println("Entering ISR 1");
//    GPIOC_PTOR = 1 << 5;
  #endif

  #ifdef TEST_MODE

    // Serial1.println("RR1");
    // Serial1.println(global_position);

  #endif

  if((global_position - stripe_count * TWO_STRIPE_DISTANCE) < TWO_STRIPE_DISTANCE_WITH_TOLERANCE )
  {           
    #ifdef DEBUG_MODE
    //  Serial1.println("RR1");
    #endif

    //isr1_count++;
    //stop timer1 if stripe_count >=1
    if(!flag)
    {
      if(stripe_count >= 1)
      {
        timer1.end();
        t2 = (TWO_SENSOR_DISTANCE_WITH_TOLERANCE/global_velocity)*1000000;
        
        #ifdef DEBUG_MODE
        //  Serial.print("t2: ");
        //  Serial.println(t2/1000000.0,6);
        #endif
        timer2.begin(timer2_isr,t2);
      }
      flag = 1;
      //start timer2 if stripe_count >=1
    }

    if(timer3_status)
      timer3.end();
  }
  else
    #ifdef DEBUG_MODE
      Serial1.println("CRITICAL RR1");
    #endif
}

void rr2_isr(){
  //Serial.println("R2");
  //Serial.println(stripe_count);
  //Serial.println(global_position);
  #ifdef DEBUG_MODE
//    Serial1.println("Entering ISR 2");
//    GPIOC_PTOR = 1 << 5;
  #endif

  #ifdef TEST_MODE
    // taskENTER_CRITICAL();
    //  Serial1.println("RR2");
    //  Serial1.println(global_position);
    // taskEXIT_CRITICAL();
  #endif

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  UBaseType_t uxSavedInterruptStatus;
  if((global_position - stripe_count * TWO_STRIPE_DISTANCE) < TWO_STRIPE_DISTANCE_WITH_TOLERANCE )
  {
    #ifdef DEBUG_MODE
//      GPIOC_PTOR = 1 << 5;
    #endif

    //isr2_count++;
    if(!flag)
      rr1_missed();//call function
    //stop timer2 if stripe_count>=1
    else if(flag)
    {
      if(stripe_count >= 1)
        timer2.end();
      flag = 0;
      
      uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();

      stripe_count++;      
      global_position = stripe_count*TWO_STRIPE_DISTANCE;// in feet
      if(stripe_count == 16){
          sys_state_change.buf[0]= LevAndBraking;
          system_state = LevAndBraking;
          Can0.write(sys_state_change);
          xHigherPriorityTaskWoken=xTaskResumeFromISR(BPID);
          portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
      }
      taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );
    }

    t1 = (TWO_STRIPE_DISTANCE/global_velocity)*1000000;
    
    #ifdef DEBUG_MODE
//      Serial.print("t1: ");
//      Serial.println(t1/1000000,6);
    #endif  
    
    timer1.begin(timer1_isr,t1);
    //Serial.println(global_position);
    if(timer3_status)
      timer3.end();
  }
  else
    #ifdef DEBUG_MODE
      Serial1.println("CRITICAL RR2");
    #endif

}


void timer1_isr()// called when timer1 expires
{
  //cli();
  timer1.end();
  if((global_position - stripe_count*TWO_STRIPE_DISTANCE)<TWO_STRIPE_DISTANCE_WITH_TOLERANCE )
    {
      t3 = 15.24/global_velocity;
      timer3.begin(timer3_isr,t3);
      timer3_status = 1;
    }
  else
    Serial.println("//CRITICAL Timer1");
}

void timer2_isr()// called when timer2 expires
{   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  UBaseType_t uxSavedInterruptStatus;
    //cli();
    timer2.end();
    //Notify base station that RRS2 missed a strip
    Serial.println("Notify Base Station: RRS2 missed a strip");
    //Serial.println(t2);
    counter1 ++;
    flag = 0; 

//    taskENTER_CRITICAL();
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
    stripe_count ++;
    global_position = stripe_count*TWO_STRIPE_DISTANCE;
    
    if(stripe_count == 16){
      sys_state_change.buf[0]= LevAndBraking;
      system_state = LevAndBraking;
      Can0.write(sys_state_change);
      xHigherPriorityTaskWoken=xTaskResumeFromISR(BPID);
      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
    taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );
//    taskEXIT_CRITICAL();

    timer1.begin(timer1_isr,t1);
    //sei();
}

void timer3_isr()
{
  cli();
  timer3_status = 0;
  timer3.end();
  t3 = 15.24/global_velocity;
  timer3.begin(timer3_isr,t3);
  timer3_status = 1;
  //Serial.println("Timer 3 expired");
  sei();
}

void rr1_missed()
{
  //notify base station
  Serial.println("Notify Base Station: RR1 missed a strip");
  timer1.end();
  flag = 0;
  counter2 ++;
  stripe_count++;
  timer1.begin(timer1_isr,t1);
}
  
//-----------------------------------Retro Reflective Sensor Code ends and State Transition code begins-----------------------------------------------------------

/*
0. Low Voltage Electronics  //Do nothing
1. Idle   //Update system state in the frame handler
2. Ready  //Update system state in the frame handler
3. Awaiting Pusher Attachment //Update system state in the frame handler
4. Pushing  //Use the accelerometer value to identify state change. Change the variable and broadcast the change. Check when to start levitation
5. Levitation & Braking  //Initiate Braking PID 
6. Descent & Retraction Of Brakes //Initiate Brake Retraction
7. Rolling on Wheels //
8. Low Speed Drive
9. Pod Stop
10. Emergency Stop
11. Fault         // Failure
12. EB0
13. EB1
14. EB2
15. Power Off
*/


void Emergency (void* args){
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  system_state = 11;
  sys_state_change.buf[0]=11;
  Can0.write(sys_state_change);
}

void brakingPID(void* args){
  
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS( 100 );
  xLastWakeTime = xTaskGetTickCount();
  
  float local_position =0;
  float local_velocity =global_velocity;
  float local_acceleration =global_acceleration;
  float des_velocity = 0;
  float actual_int_velocity = 0;
  float actual_diff_velocity = 0;
  float diff_velocity = 0;
  float int_velocity = 0;
  float ctrl_output = 0;
  float commanded_velocity = 0;
  float scaled_position = 0;
  float brakepad_gap = 0;
  
  float p1 =      -1.012;
  float p2 =      -10.72; 
  float p3 =      -45.14;  
  float p4 =      -94.41;  
  float p5 =      -96.96;  
  float p6 =      -34.51;  
  float p7 =        9.32;  
  float p8 =      -2.169;  
  float p9 =      -26.62;  
  float p10 =      44.34;

  float C   =   4.188273875650571e+25;   //%constant defined assuming integral of velocity at start, x = 457.69m is 0

  brake_msg.ext=0;
  brake_msg.id = BRAKING; //12
  brake_msg.len = 1;

  vTaskSuspend(NULL);
 
  while (1){
        // x1 -> Current position of the pod
    local_position = global_position;
    scaled_position = float((local_position - 1065))/215.4;
    vTaskSuspend(NULL);
    //Based on the values recorded in the last period, we get the actual_int_velocity and actual_diff_velocity

    actual_int_velocity = actual_int_velocity + 0.1*pow(local_velocity,2);     //calculate the actual integral of velocity wrt distance
    actual_diff_velocity = local_acceleration/local_velocity;      //calculate the actual differential of velocity wrt to distance          

    local_velocity=global_velocity; 
    local_acceleration = global_acceleration;
 
    des_velocity = p1*pow(scaled_position, 9) + p2*pow(scaled_position, 8) + p3*pow(scaled_position, 7) + p4*pow(scaled_position, 6) + p5*pow(scaled_position, 5) + p6*pow(scaled_position, 4) + p7*pow(scaled_position, 3) + p8*pow(scaled_position, 2) + p9*scaled_position + p10;
    diff_velocity = 9*p1*pow(scaled_position, 8) + 8*p2*pow(scaled_position,7) + 7*p3*pow(scaled_position,6) + 6*p4*pow(scaled_position, 5) + 5*p5*pow(scaled_position, 4) + 4*p6*pow(scaled_position, 3) + 3*p7*pow(scaled_position, 2) + 2*p8*scaled_position + p9; 
    int_velocity = (p1*pow(scaled_position, 10))/10 + (p2*pow(scaled_position, 9))/9 + (p3*pow(scaled_position, 8))/8 + (p4*pow(scaled_position, 7))/7 + (p5*pow(scaled_position, 6))/6 + (p6*pow(scaled_position, 5))/5 + (p7*pow(scaled_position, 4))/4 + (p8*pow(scaled_position,3))/3 + (p9*pow(scaled_position, 2))/2 + p10*scaled_position + C;
    
    ctrl_output = Kp*(des_velocity - local_velocity) + Ki*(int_velocity - actual_int_velocity) + Kd*(diff_velocity - actual_diff_velocity);       
    
    commanded_velocity = local_velocity + ctrl_output;    //  Commanded velocity is the velocity required at the next period. 
    

    if (local_acceleration >= -750/POD_MASS)  //The equation are relating the current velocity and the brake pad gap. So we are taking the value for the current drag.
      brakepad_gap = (ctrl_output*POD_MASS + 659.09)/20.59;
    else
      brakepad_gap = (ctrl_output*POD_MASS - 60.6*commanded_velocity + 6354.125)/(400.266 - 4.04*commanded_velocity);

    if (brakepad_gap >= 32)
      brakepad_gap = 32;
    else if (brakepad_gap <= 10)
      brakepad_gap = 10;
    
    //Assume 1 means going in and -1 means going out
    if(braking_sep_avg - brakepad_gap > 0){
      pid_direction = 1;
    } 
    else{
      pid_direction = -1;
    }   

    if (pid_direction==-1){
      brake_msg.buf[0] = 3;
    }
    else{
      brake_msg.buf[0] = 1;
    }
    
    needed_gap_height = brakepad_gap;

    Can0.write(brake_msg);     
    vTaskDelayUntil( &xLastWakeTime, xPeriod );
  }
}


/*
1st bit: 0-> Enable and 1-> Disable  //Enable
2nd bit: 0-> Move in and 1-> Move out //Direction
3rd bit: 0-> 250 and 1-> 560

0: 000: Enable, Move in and stay (250) 
1: 001: Enable, Move in and ramp (560)
2: 010: Enable, Move out and stay (250)
3: 011: Enable, Move out and ramp (560)
4: 100: Disable
*/

void brakingValueCheck(void* args){ 
    
/*
1. 
Get the lts reading and continuously check if it is equal to the brake pad gap that we want. Continue spinning the motor till the lts reading is not equal to brakepad_gap.
When the lts reading equals brake pad gap within the time frame, make rpm = 250 immediately.

2.
The direction of the motor is given by ctrl_output/mod(ctrl_output). 
If +1 -> retracrt brakes
If -1 -> brakes engage
Essentially toggling the direction pin.

3. If the LTS reading is 32 or 10 and the brake pad gap is 32 or 10 toggle the enable pin. 

4. When the motor changes its direction or starts from rest, start the rpm from 250 and ramp it up till 560 and then hold. Do this at the rate of 620 rpm per seconds. => 6.2 in 10 milliseconds.

5. Frequency = rpm * 400 /60 for half stepping. 
*/

// Braking actuation data packet: 250 - 560 : 2 byte data packet. 10 bit + 1 bit enable + 1 bit direction
// Bit 0-13: Frequency and bit 14 direction and bit 15: enable

// Start processing this task
  
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS( 10 );
  xLastWakeTime = xTaskGetTickCount();



  for(;;){
    //Assume 1 means going in and -1 means going out
    if (pid_direction == -1 && braking_sep_avg >= 32){
      brake_msg.buf[0] = 4;
      Can0.write(brake_msg); 
    }

    else if (pid_direction == 1 && braking_sep_avg <= 10){
      brake_msg.buf[0] = 4;
      Can0.write(brake_msg); 
    }    
    
    else if (pid_direction == -1 && braking_sep_avg >= needed_gap_height){
      brake_msg.buf[0] = 2;
      Can0.write(brake_msg); 
    }

    else if(pid_direction == 1 && braking_sep_avg <= needed_gap_height){
      brake_msg.buf[0] = 0;
      Can0.write(brake_msg); 
    }

    vTaskDelayUntil( &xLastWakeTime, xPeriod );
  }
}


void lowSpeedDrive(void *args){
  float local_position;
  float local_velocity;
  float local_acceleration;
  int flag=0;
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS( 100 );
  xLastWakeTime = xTaskGetTickCount();
  
  lsd_msg.ext = 0;
  lsd_msg.id = LSDRPM;
  lsd_msg.len = 1;
  
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  
  for(;;){
    local_acceleration = global_acceleration;
    local_position = global_position;
    local_acceleration = global_acceleration;
    
    if (flag==0 && (local_position < LOW_SPEED_POSITION_THRESHOLD)){
      lsd_msg.buf[0] = 10;
      flag=1;
      Can0.write(lsd_msg);
    }

    else if (local_position >= LOW_SPEED_POSITION_THRESHOLD){
      lsd_msg.buf[0] = 0;
      Can0.write(lsd_msg);
      vTaskDelete(NULL);
    }

    vTaskDelayUntil( &xLastWakeTime, xPeriod );
  }
}


void linearActuator(void* args){
  float local_velocity;
  float local_position;
  float time_to_deploy;

  LinAct_msg.ext = 0;
  LinAct_msg.id = LINACT;
  LinAct_msg.len = 1;

  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

  local_position = global_position;
  local_velocity = global_velocity;

  #ifdef DEBUG_MODE
    while(local_velocity == 0){
      Serial1.println("Zero Velocity Error");
    }
  #endif

  time_to_deploy = (PUSHER_DETACHMENT_POSITION - local_position)/local_velocity;

  while (time_to_deploy > 4){
    vTaskDelay(pdMS_TO_TICKS(500));
    local_position = global_position;
    local_velocity = global_velocity;
    time_to_deploy = (PUSHER_DETACHMENT_POSITION - local_position)/local_velocity;
  }
  
  #ifdef DEBUG_MODE
  //  GPIOC_PTOR = 1 << 5;
  #endif
  LinAct_msg.buf[0]=0x01;  //Enable
  //(lsbits --> direction, msbits-->enable);
  Can0.write(LinAct_msg);         //Begin retraction of wheels
  vTaskDelete( NULL );

}


//------------------------------------------State Transition code ends and CAN / Data Logging code begins--------------------------------------------------------------------------------------------

 void init_can(){
    Can0.begin(CAN_BAUD);
    Can0.attachObj(&canListener1);
    canListener1.attachGeneralHandler();
    sys_state_change.ext = 0;
    sys_state_change.id = STATECHNGID; //5
    sys_state_change.len = 1;
    
    sys_state_ack.ext = 0;
    sys_state_ack.id = SYSSTATEACK; //9
    sys_state_ack.len = 1;
    sys_state_ack.buf[0] = 1;

    lts_packet1.ext = 0;
    lts_packet1.id = LTSB12; //19
    lts_packet1.len = 8;
    
    lts_packet2.ext = 0;
    lts_packet2.id = LTSB34; //20
    lts_packet2.len = 8;
    
    attitude_acc.ext = 0;
    attitude_acc.id = ACCGYRO; //16
    attitude_acc.len = 8;
    
    pos_vel.ext = 0;
    pos_vel.id = POSVELO; //17
    pos_vel.len = 8;
    
    stripe_count_msg.ext = 0;
    stripe_count_msg.id = RRSCOUNT; //18
    stripe_count_msg.len = 1;
    
    #ifdef DEBUG_MODE
      Display.println("Navigation: CAN initialised");
      Display.flush();
    #endif
}

void init_sdcard(){
  if (SD.begin(chipSelect)) {
    Display.println("Navigation: SD card initialised");
  } 
  else{
    Display.println("Navigation: SD card not initialised"); 
    initialise_status=0;
  }
   dataFile = SD.open("datalog.txt", FILE_WRITE);    
   if (dataFile) {
       Display.println("Data Logging file ready");
   }
   else{
     Display.println("Unable to open file for Data logging");
     initialise_status=0;
   }
   Display.flush();
}


void dataLogging(void* args){
  uint16_t local_yaw = 0;
  uint16_t local_pitch = 0;
  uint16_t local_roll = 0;

  uint16_t local_acceleration =10;
  uint16_t local_velocity =2 ;
  uint16_t local_position =3;
  init_sdcard();

  for(;;){
    if (dataFile) {    

      digitalWriteFast(21, HIGH);
      Display.println("Logging Data");

//      Display.println("Entering Data Logging Task");
      xSemaphoreTake( accMutex, portMAX_DELAY );
      local_velocity = global_velocity;
      local_acceleration = global_acceleration;
      local_position = global_velocity;
      xSemaphoreGive( accMutex );

      // digitalWriteFast(29, HIGH);
      // vTaskDelay(pdMS_TO_TICKS(1));
      // digitalWriteFast(29, LOW);
      // vTaskDelay(pdMS_TO_TICKS(1));
      xSemaphoreTake( attitudeMutex, portMAX_DELAY );
      local_yaw = global_yaw;
      local_pitch = global_pitch;
      local_roll = global_roll;
      xSemaphoreGive( attitudeMutex );
      // vTaskDelay(pdMS_TO_TICKS(1));
      // digitalWriteFast(29, HIGH);
      // vTaskDelay(pdMS_TO_TICKS(1));
      // digitalWriteFast(29, LOW);

      dataFile.print(xTaskGetTickCount()); dataFile.print(": \r\n");
      dataFile.print("Acceleration: ");  dataFile.print(local_acceleration); dataFile.print("\r\n");
      dataFile.print("Velocity: "); dataFile.print(local_velocity); dataFile.print("\r\n");
      dataFile.print("Position: "); dataFile.print(local_position); dataFile.print("\r\n");
      dataFile.print("Yaw: "); dataFile.print(local_yaw); dataFile.print("\r\n");
      dataFile.print("Pitch: "); dataFile.print(local_pitch); dataFile.print("\r\n");
      dataFile.print("Roll: "); dataFile.print(local_roll); dataFile.print("\r\n");
      dataFile.flush();

      // digitalWriteFast(29, HIGH);
      // vTaskDelay(pdMS_TO_TICKS(1));
      // digitalWriteFast(29, LOW);
      // vTaskDelay(pdMS_TO_TICKS(1));

      *attitude_acc.buf = local_yaw;
      *(attitude_acc.buf+2)=local_pitch;
      *(attitude_acc.buf+4)=local_roll;
      *(attitude_acc.buf+6)=local_acceleration;
      // *(pos_vel.buf) = local_position;
      // *(pos_vel.buf+32) = local_velocity;

      // digitalWriteFast(29, HIGH);
      // vTaskDelay(pdMS_TO_TICKS(1));
      // digitalWriteFast(29, LOW);

      Can0.write(attitude_acc);
  //    Can0.write(pos_vel);
      
      digitalWriteFast(21, LOW);    
    }
    else{
      Display.println("Error is Logging Data");
    }
    dataString = "";
 //   Display.println("Exiting Data Logging Task");
    vTaskDelay(pdMS_TO_TICKS(DataLogTimerPeriod));
  }  
}


void ledTask(void* args){
  Display.println("Entering LED task");
  digitalWrite(13, HIGH);
  vTaskDelay(pdMS_TO_TICKS(500));
  digitalWrite(13, LOW);
  Display.println("Exiting LED Task");
  vTaskDelay(pdMS_TO_TICKS(500));
}


//-----------------------------------------Testing code-------------------------------------------------------------------------------

#ifdef TEST_MODE

void serialDebug(void* args){
  for(;;){
    Display.println(system_state);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void stateTransition(void* args){
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  for(;;){
    if(system_state == 3){
      #ifdef DEBUG_MODE
//        Serial1.println("Entering State Transition");
//        GPIOC_PTOR = 1 << 5;
      #endif
      vTaskDelay(pdMS_TO_TICKS(1000));
      global_acceleration1 = 10;
      global_acceleration2 = 10;                 
    }

    if(system_state == 4){
      #ifdef DEBUG_MODE
//        Serial1.println("RR TB");
      #endif

      pinMode(rr1_pin,INPUT);
      pinMode(rr2_pin,INPUT);
      attachInterrupt(rr1_pin,rr1_isr,RISING);
      attachInterrupt(rr2_pin,rr2_isr,RISING);

      vTaskDelay(pdMS_TO_TICKS(1000));
      stripe_count = 15;
      global_position = 15*TWO_STRIPE_DISTANCE;
      global_velocity = 40; 
      flag=0;
      timer1.begin(timer1_isr, TWO_STRIPE_DISTANCE/40);
      vTaskDelay(pdMS_TO_TICKS(100));
      digitalWrite(11, HIGH);
      digitalWrite(12, HIGH);

      timer1.end();

      detachInterrupt(rr1_pin);
      detachInterrupt(rr2_pin);

      #ifdef DEBUG_MODE
//        Serial1.println("TB Exit");
      #endif
     
      vTaskDelay(pdMS_TO_TICKS(1000));

      global_velocity = 15;
      global_position = 1000;

      vTaskDelay(pdMS_TO_TICKS(1000)); 

      global_velocity = 10;
      global_position = 1240;

    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

#endif


/*
--------------------------------------------------------------------------------------------------------------------------------------
                                          Main begins
--------------------------------------------------------------------------------------------------------------------------------------
*/

int main(void) {

  Display.begin(9600);
  Display.println("Starting initialisation");

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delayMicroseconds(1000000);
  digitalWrite(13, LOW);
  delayMicroseconds(1000000);

  pinMode(29, OUTPUT);
  pinMode(21, OUTPUT); 
  digitalWrite(21, HIGH);
  digitalWrite(21, LOW);

//  init_rr();
//  init_acc();
//  init_lts();
  init_can();

  accMutex = xSemaphoreCreateMutex();
  dmpMutex = xSemaphoreCreateBinary();
  brakingPIDValueSem = xSemaphoreCreateBinary();
  attitudeMutex = xSemaphoreCreateMutex();
  PKTHDLE[1] = xSemaphoreCreateBinary();
  PKTHDLE[2] = xSemaphoreCreateBinary();
   
  system_state = 0;
  
  xTaskCreate(CANReceive1, "CAN State Change Packet", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
//  xTaskCreate(retro_state_machine, "RR setup", configMINIMAL_STACK_SIZE, NULL, 6, NULL);
  
 // xTaskCreate(Emergency, "Emergency", configMINIMAL_STACK_SIZE, NULL, 4, &E);
//  xTaskCreate(canSend, "CANS", configMINIMAL_STACK_SIZE, NULL, 1, &CANS);
//  xTaskCreate(dataLogging, "DataL", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
//  xTaskCreate(ledTask, "LEDTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(brakingPID, "PID", configMINIMAL_STACK_SIZE, NULL, 4, &BPID);
//  xTaskCreate(brakingValueCheck, "Braking Computation", configMINIMAL_STACK_SIZE, NULL, 1, &BrComp);
 // xTaskCreate(brakingStop, "BrStop", configMINIMAL_STACK_SIZE, NULL, 1, &BrStop);
//  xTaskCreate(lts, "LaserT", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
//  xTaskCreate(accelerometer, "Acc", configMINIMAL_STACK_SIZE, NULL, 2, &ACCEL);
 // xTaskCreate(stateEstimate, "DeadR", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(linearActuator, "Linear Actuator", configMINIMAL_STACK_SIZE, NULL, 3, &LinAct); //Prty
  xTaskCreate(poseComputation, "Dead Reckoning", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  xTaskCreate(lowSpeedDrive, "Low Speed Drive", configMINIMAL_STACK_SIZE, NULL, 3, &LSD);

//    Timer1 = xTimerCreate("Timer1", pdMS_TO_TICKS(10000), pdFALSE, (void *)2, Task3);
//    Timer2 = xTimerCreate("Timer2", pdMS_TO_TICKS(10000), pdFALSE, (void *)2, Task7);
//    Timer3 = xTimerCreate("Timer3", pdMS_TO_TICKS(10000), pdFALSE, (void *)2, Task3);
  stateAckFailureTimer = xTimerCreate("State Ack Failure", CAN_ACK_FAILURE_PERIOD, pdFALSE, 0, canAckFailureCallback);

#ifdef TEST_MODE
  xTaskCreate(stateTransition, "Internal State Transition Test", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
  xTaskCreate(serialDebug, "State change Debugger", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
#endif

/*
-----------------------------------------------------------------------------------------------------------------
Wait for the pod start command
-----------------------------------------------------------------------------------------------------------------
*/


  Display.println("Scheduler Starting");

  vTaskStartScheduler();
  for(;;);  
  return 0;
}
