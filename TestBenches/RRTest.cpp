#include <kinetis.h>
#include <stdlib.h>
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
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "i2c_t3.h"
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <cmath>

#define Display Serial1
#define Serial_BAUD 9600
#define CAN_BAUD 500000
#define Acc_Pin 14
#define RR_Pin1 2
#define RR_Pin2 3
#define AccDelay 100
#define GyroDelay 100
#define DataLogTimerPeriod 1000  
#define DEBUG_MODE 1
#define PUSHER_DETACHMENT_POSITION 1600

volatile int system_state=0;

#define AccMin -2
#define AccMax 2
#define YawMin -0.5
#define YawMax 0.5
#define PitchMin -1.3
#define PitchMax  1.3
#define RollMin -2
#define RollMax 2

#define  XGyroOffset 177
#define  YGyroOffset -33
#define  ZGyroOffset -52
#define  ZAccelOffset 870
#define  YAccelOffset -1525
#define  XAccelOffset -108 // 1688 factory default for my test chip



TaskHandle_t TASK1=NULL, TASK2=NULL, TASK3=NULL, TASK4=NULL, TASK5=NULL, TASK6=NULL, TASK7=NULL;              //Declare Task Handles
TaskHandle_t PU = NULL;
TaskHandle_t LinAct=NULL, BPID=NULL, brRetraction=NULL, BrStop=NULL, E=NULL, CANS=NULL;

const int chipSelect = BUILTIN_SDCARD;
const float DIS_BW_2_STRIPES=10;
const float DIS_BW_2_SENSORS=10;

bool initialise_status=1;
volatile int16_t global_acceleration;
volatile uint32_t global_velocity;
volatile uint32_t global_position=0;
volatile uint16_t global_yaw;
volatile uint16_t global_pitch;
volatile uint16_t global_roll;
volatile uint8_t braking_sep_1;
volatile uint8_t braking_sep_2;
volatile uint8_t braking_sep_3;
volatile uint8_t braking_sep_4;
volatile uint8_t braking_sep_avg;
volatile uint8_t stripe_count = 0; //for counting the number of strips passed by retroreflective sensor

String dataString="";

MPU6050 mpu;
bool blinkState = false;
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
EventGroupHandle_t grp;//eventGroup handles for ISR and their related functions
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
uint16_t counter = 0;
volatile bool mpuInterrupt = false;

Adafruit_BNO055 bno1 = Adafruit_BNO055(55,0x28);
imu::Vector<3> eul1;
/*
IntervalTimer rrTimer1;
IntervalTimer rrTimer2;
IntervalTimer rrTimer3; 
*/
int sensor1_error_counter=0;
int sensor2_error_counter=0;
int timer3_error_counter=0;

TimerHandle_t dataLogTimer;
File dataFile;
SemaphoreHandle_t accMutex;
SemaphoreHandle_t attitudeMutex;

CAN_message_t sys_state_change;
CAN_message_t sys_state_ack;
CAN_message_t health_ack;
CAN_message_t lts_packet1;
CAN_message_t lts_packet2;
CAN_message_t attitude_acc;
CAN_message_t pos_vel;
CAN_message_t stripe_count_msg;
CAN_message_t LinAct_msg;
CAN_message_t brake_msg;
CAN_message_t lsd_msg;

volatile int flag=0;
volatile int rr_flag=0;

void Task1(void* args);
void Task2(void* args);
void Task3(void* args);
void Task4(void* args);
void Task5(void* args);
void Task6(void* args);
void Task7(void* args);

void posUpdate (void* args);

class NavCANListener : public CANListener 
{
public:
  bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

NavCANListener canListener1;

bool NavCANListener::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
      /*
0. Low Voltage Electronics 
1. Idle
2. Ready
3. Awaiting Pusher Attachment
4. Pushing
5. Levitation & Breaking
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

  switch(frame.buf[0]){
    case 0: {         //Power On; Low Voltage Electronics powered on
    //xTaskNotifyGive(PKTHDLE1);
    system_state = 0;
    break;    
    }
    case 1: {         //Idle; from Power node (High Voltage stuff is on)
      system_state = 1; 
    break;    
    }
    case 2: {         //Ready; from Comm Node (signal that Health Check is Cool)
      system_state = 2;
      break;
    }
    case 3: {         //Awaiting Pusher Attachment; from Comm Node (signal from UI to launch Pod)
      system_state = 3;
      break;
    }
    case 4: {         //Pushing
      //report Error
      break;
    }
    case 5: {         //Levitation & Breaking;          
      //report Error
      break;
    }
    case 6: {
      //report Error   //Descent & retraction of brakes
      break;
    }
    case 7: {          //Rolling on Wheels     
      system_state = 7;
      break;
    }  
    case 8: {         //Low Speed Drive
      //report Error
      break;
    }
    case 9: {         //Pod Stop
      //report Error
      break;
    }
    case 10: {        //Emergency Stop
      //report Error
      break;
    }
    case 11: {        //EB0 
      system_state = 11;
      break;
    }
    case 12: {        //EB1
      system_state = 12;
      break;
    }
    case 13: {        //EB2
      system_state = 13;
      break;
    }
    case 14: {        //Power Off
      system_state = 14;
      break;
    }
  }
  return true;
}

/*
-----------------------------------------------------------------------------------------------------------------
Tasks are defined from here
-----------------------------------------------------------------------------------------------------------------
*/

void canSend (void* args){

}

void dmpDataReady() {
  mpuInterrupt = true;
  //Need to ask about xEventGroup!
}

void accelerometer(void* args){
  uint16_t local_acceleration =0;
  uint16_t local_velocity =0 ;
  uint16_t local_position =0;
  float time_delay;
  float old_time =0;

  pinMode(5, INPUT_PULLUP);
  attachInterrupt(5, dmpDataReady, CHANGE);

  for(;;){
    xSemaphoreTake( accMutex, portMAX_DELAY );
    local_velocity=global_velocity;
    local_position=global_position; 
    xSemaphoreGive( accMutex );
    
    xEventGroupWaitBits(grp,1UL<<1UL,pdTRUE,pdTRUE,portMAX_DELAY);

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10)) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Display.println("Navigation Accelerometer: FIFO overflow!");

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
        Display.println(fifoCount);

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;
         // display initial world-frame acceleration, adjusted to remove gravity
          // and rotated based on known orientation from quaternion
          mpu.dmpGetQuaternion(&q, fifoBuffer);
          mpu.dmpGetAccel(&aa, fifoBuffer);
          mpu.dmpGetGravity(&gravity, &q);
          mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
          mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
          Display.println("areal\t");
          Serial.print("\t");
          Serial.print("\t");
          if(counter>=2000){
            local_acceleration = (aaWorld.x)*9.8/16384;
          }
        counter++;
      }

    if (local_acceleration>AccMin && local_acceleration<AccMax){
        time_delay = micros() - old_time; // have to initialise old time variable and see the format of micros()
        local_velocity += local_acceleration * time_delay;
        local_position += local_velocity * time_delay;
        xSemaphoreTake( accMutex, portMAX_DELAY );
        global_velocity=local_velocity;
        global_position=local_position; 
        xSemaphoreGive( accMutex );
        old_time = micros();  
    }
    else {
      xTaskNotifyGive(E);  // Send notification to Emergency Task
    }

    if (system_state==3 && local_acceleration > 0.1){
        system_state = 4;
        sys_state_change.buf[0]=4;
        Can0.write(sys_state_change);
        xTaskNotifyGive(LinAct);      //In state 4
    }

    else if (system_state==5 && local_velocity < 20){
        system_state = 6;
        sys_state_change.buf[0]=6;
        Can0.write(sys_state_change);
        vTaskDelete(BPID);                  //Stop braking PID
        xTaskNotifyGive(brRetraction);      //In state 6, start Descent & brake Retraction
        //Delete the PID task
    }

    else if (system_state==7){ 
      if (local_velocity == 0){         //How to precisely say that pod has stopped?
        system_state = 9;
        sys_state_change.buf[0]=9;
        Can0.write(sys_state_change);
        xTaskNotifyGive(BrStop);
      }
      if (local_position <= 4100 && local_velocity <= 2){
        system_state = 8;         //Low Speed Drive
        sys_state_change.buf[0]=8;
        Can0.write(sys_state_change);
        //eqn to calculate DC motor RPM
        lsd_msg.buf[0]=0;
        lsd_msg.buf[1]=0;
        lsd_msg.buf[2]=0;
        lsd_msg.buf[3]=0;
        lsd_msg.buf[4]=0;
        lsd_msg.buf[5]=0;
        lsd_msg.buf[6]=0;
        lsd_msg.buf[7]=0;
        Can0.write(lsd_msg);
      }  

      else if (local_position > 4100 && local_velocity > 2){
        system_state = 11;      //Emergency Braking 0 a.k.a. EB0
        sys_state_change.buf[0]=11;
        Can0.write(sys_state_change);
      }
    }

    if (system_state==8 && local_velocity==0){
        system_state = 9;
        sys_state_change.buf[0]=9;
        Can0.write(sys_state_change);
    }

    if (system_state==11 && local_velocity==0){
        system_state = 10;
        sys_state_change.buf[0]=10;
        Can0.write(sys_state_change);
    }

    if (system_state==12 && local_velocity==0){
        system_state = 10;
        sys_state_change.buf[0]=10;
        Can0.write(sys_state_change);
    }

    if (system_state==13 && local_velocity==0){
        system_state = 10;
        sys_state_change.buf[0]=10;
        Can0.write(sys_state_change);
    }

    vTaskDelay(pdMS_TO_TICKS(AccDelay));
  }
}


void gyro(void* args){
  volatile uint16_t local_yaw;
  volatile uint16_t local_pitch;
  volatile uint16_t local_roll;

  for(;;){
    xSemaphoreTake( accMutex, portMAX_DELAY );
    local_yaw = global_yaw;
    local_pitch = global_pitch; 
    local_roll = global_roll; 
    xSemaphoreGive( accMutex );
    if ((local_pitch> PitchMin && local_pitch < PitchMax) && (local_yaw>YawMin && local_yaw<YawMax)){
      eul1 = bno1.getVector(Adafruit_BNO055::VECTOR_EULER);
      local_yaw = eul1[0];
      local_pitch = eul1[1];
      local_roll = eul1[2];
      xSemaphoreTake( accMutex, portMAX_DELAY );
      global_yaw = local_yaw;
      global_pitch = local_pitch;
      global_roll = local_roll;
      xSemaphoreGive( accMutex );
      vTaskDelay(pdMS_TO_TICKS(GyroDelay));
    }
    else{
      xTaskNotifyGive(E);     
    }
  }
}

void brakingStop(void* args){
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  brake_msg.buf[0] = 0;
  brake_msg.buf[1] = 0;
  brake_msg.buf[2] = 0;
  brake_msg.buf[3] = 0;
  brake_msg.buf[4] = 0;
  brake_msg.buf[5] = 0;
  brake_msg.buf[6] = 0;
  brake_msg.buf[7] = 0;
}

void dataLogging(void* args){
  if (dataFile) {
    Display.print("Waiting for mutex\r\n");
    xSemaphoreTake( accMutex, portMAX_DELAY );
    Display.print("Mutex acquired\r\n");
    dataFile.print(xTaskGetTickCount());
    dataFile.print(": \r\n");
    dataFile.print("Acceleration: ");
    dataFile.print(global_acceleration);
    dataFile.print("\r\n");
    dataFile.print("Velocity: ");
    dataFile.print(global_velocity);
    dataFile.print("\r\n");
    dataFile.print("Position: ");
    dataFile.print(global_position);
    dataFile.print("\r\n");
    dataFile.print("Yaw: ");
    dataFile.print(global_yaw);
    dataFile.print("\r\n");
    dataFile.print("Pitch: ");
    dataFile.print(global_pitch);
    dataFile.print("\r\n");
    dataFile.print("Roll: ");
    dataFile.print(global_roll);
    dataFile.print("\r\n");
    dataFile.flush();
    xSemaphoreGive( accMutex );
    Display.print("Mutex given\r\n");

    Display.print("Yay!!!!\r\n");
  }
  else{
    Display.println("Noooooooooo!!!");
  }
  dataString = "";
}


void Emergency (void* args){
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  Serial1.println("Entering Emergency Task");
  system_state = 11;
  sys_state_change.buf[0]=11;
  Can0.write(sys_state_change);
}

void brakingPID(void* args){
  float Kp, Ki, Kd;
  uint32_t local_position;
  uint32_t local_velocity;
  uint32_t local_acceleration;
  uint32_t desired_position;
  uint32_t desired_velocity;
  double x, x1, p1, p2, p3, p4, p5, p6, vel;
  
  x1 = 350.2;
  x = (x1 - 420.9)/215.9;
  p1 =     -0.9772 ;
  p2 =      -2.929  ;
  p3 =     -0.8259 ;
  p4 =      -3.175 ;
  p5 =      -20.88 ;
  p6 =       69.97 ;

  brake_msg.ext=0;
  brake_msg.id = 0xC; //12
  brake_msg.len = 1;

  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  Serial1.println("Entering BPID Task");
  xSemaphoreTake( accMutex, portMAX_DELAY );
  local_velocity=global_velocity; 
  local_position = global_position;
  local_acceleration = global_acceleration;
  xSemaphoreGive( accMutex );


  if((local_velocity>75)&&(local_velocity<=80)){
    Kp=3;
    Ki=0.1;
    Kd=0.01;    
  }
  else if((local_velocity>70)&&(local_velocity<=75)){
    Kp=3;
    Ki=0.1;
    Kd=0.01;    
  }

  else if((local_velocity>65)&&(local_velocity<=70)){
    Kp=3.5;
    Ki=0.2;
    Kd=0.01;    
  }
  
  else if((local_velocity>60)&&(local_velocity<=65)){
    Kp=3.5;
    Ki=0.3;
    Kd=0.01;    
  }

  else if((local_velocity>55)&&(local_velocity<=60)){
    Kp=3.7;
    Ki=0.35;
    Kd=0.01;    
  }

  else if((local_velocity>50)&&(local_velocity<=55)){
    Kp=4.5;
    Ki=0.45;
    Kd=0.01;    
  }

  else if((local_velocity>45)&&(local_velocity<=50)){
    Kp=4.8;
    Ki=0.48;
    Kd=0.01;    
  }

  else if((local_velocity>40)&&(local_velocity<=45)){
    Kp=5.6;
    Ki=0.55;
    Kd=0.01;    
  }
  else{
    xTaskNotifyGive(CANS);
  }
  
  while (braking_sep_avg > 11){
    xSemaphoreTake( accMutex, portMAX_DELAY );
    local_velocity=global_velocity; 
    local_position = global_position;
    local_acceleration = global_acceleration;
    xSemaphoreGive( accMutex );
  
  /*desired_velocity =  p1*pow(x,5) + p2*pow(x,4)  + p3*pow(x,3) + p4*pow(x,2) + p5*x + p6; 
    desired_position = 
    desired_acceleration = 
    
    x_err = (Kp*(desired_velocity - local_velocity) + Ki*(desired_distance - local_distance) +Kd*(desired_acceleration - local_acceleration));
    
    brake_msg.buf[0]=x_err*7;
    can0.write(brake_msg); 
    */
    vTaskDelay(pdMS_TO_TICKS(5));
  }
    /*
      Defer processing to a task that stops the motor at the specified distance and deletes the pidTask
    */
  
}

void lowSpeedDrive(void *args){

}

void linearActuator(void* args){
  uint32_t local_velocity;
  uint32_t local_position;
  float time_to_deploy;

  LinAct_msg.ext = 0;
  LinAct_msg.id = 0xA; //10
  LinAct_msg.len = 1;

  xSemaphoreTake( accMutex, portMAX_DELAY );
  local_position = global_position;
  xSemaphoreGive(accMutex);
  time_to_deploy = (PUSHER_DETACHMENT_POSITION - local_position)/local_velocity;

  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  while (time_to_deploy > 4){
    vTaskDelay(pdMS_TO_TICKS(500));
    xSemaphoreTake( accMutex, portMAX_DELAY );
    local_position = global_position;
    xSemaphoreGive(accMutex);
    time_to_deploy = (PUSHER_DETACHMENT_POSITION - local_position)/local_velocity;
  }
    
  LinAct_msg.buf[0]=1;
  Can0.write(LinAct_msg);         //Begin retraction of wheels
}


void lts (void* args){
  //Update  values of global variables braking_sep_1, braking_sep_2,braking_sep_3,braking_sep_4;
  //Compute braking_sep_avg
}

void brakeRetraction (void* args){
  ulTaskNotifyTake (pdTRUE, portMAX_DELAY);
  //send data pkt to reverse direction of the stepper motor and move it to 15mm at a constant frequency 
}


/*
----------------------------------------------------------------------------------------------------------------------------------------
Retro Reflective Sensor Tasks
----------------------------------------------------------------------------------------------------------------------------------------
*/


TimerHandle_t Timer1;
TimerHandle_t Timer2;
TimerHandle_t Timer3;
bool first_time_flag=0;
/*
Timer1 : 100 feet/V + __

Timer2 : 3 feet/V + ___

Timer3 : 
*/
void isrService1(){
  if(flag==0){
    xTaskNotifyGive(TASK1);
  }
  else if(flag==3){
    xTaskNotifyGive(TASK4);
  }
  else
    xTaskNotifyGive(CANS);
}

void isrService2(){
  if(flag==0){
    xTaskNotifyGive(TASK6);
  }
  else if(flag==1){
    xTaskNotifyGive(TASK2); 
  }
  else if(flag==3){
    xTaskNotifyGive(TASK5);
  }   
  else
    xTaskNotifyGive(CANS);
}


void Task1(void* args){
    double local_velocity;
    uint32_t local_position;
    uint8_t local_stripe_count;
    first_time_flag = true;
    for(;;){
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
      //rrTimer1.end();
      Serial1.println("Entering Task1");
      if (!first_time_flag)
        xTimerStop(Timer1, portMAX_DELAY);
      rr_flag=1;
      xSemaphoreTake( accMutex, portMAX_DELAY );
      global_velocity=local_velocity;
      global_position=local_position;
      stripe_count=local_stripe_count;
      xSemaphoreGive( accMutex );
      //rrTimer2.begin(Task7, 5); //DIS_BW_2_SENSORS/local_velocity
      //xTimerStart(Timer2, pdMS_TO_TICKS(DIS_BW_2_SENSORS/local_velocity));
      xTimerStart(Timer2, pdMS_TO_TICKS(5000));
      first_time_flag = false;
    } 
}


void Task2(void* args){
  double local_velocity;
    uint32_t local_position;
    uint8_t local_stripe_count;
    for(;;){
      ulTaskNotifyTake (pdTRUE, portMAX_DELAY);
      //rrTimer2.end();
      Serial1.println("Entering Task2");
      xTimerStop(Timer2, portMAX_DELAY);
      rr_flag=0;
      xSemaphoreTake( accMutex, portMAX_DELAY );
      global_velocity=local_velocity;
      global_position=local_position;
      stripe_count=local_stripe_count;
    xSemaphoreGive( accMutex );
    //Update pos, vel, stripe_count
    if (stripe_count == 16){
      system_state=5;
        sys_state_change.buf[0]=5;
        Can0.write(sys_state_change);
        xTaskNotifyGive(BPID);
    }
    //rrTimer1.begin(Task3, 5); //DIS_BW_2_STRIPES/local_velocity
    //xTimerStart(Timer1, pdMS_TO_TICKS(DIS_BW_2_STRIPES/local_velocity));
    xTimerStart(Timer1, pdMS_TO_TICKS(1000));
  }
}


void Task3(void* args){
  //Read current x and stripe_count from global variables
    uint32_t local_velocity;
    uint32_t local_position;
    uint8_t local_stripe_count;
    
    for(;;){
      ulTaskNotifyTake (pdTRUE, portMAX_DELAY);
      Serial1.println("Entering Task3");
      xSemaphoreTake( accMutex, portMAX_DELAY );
      global_velocity=local_velocity;
      global_position=local_position;
      stripe_count=local_stripe_count;
      xSemaphoreGive( accMutex );
      if (local_position - local_stripe_count > 150){
        xTaskNotifyGive(E);
      }
      else{
        rr_flag=3;
        //rrTimer3.begin(Task3, 5000);
        xTimerStart(Timer3, 5000);

        timer3_error_counter+=1;
      }
    }
}


void Task4(void* args){
    uint32_t local_velocity;
    uint32_t local_position;
    uint8_t local_stripe_count;

    for(;;){
      ulTaskNotifyTake (pdTRUE, portMAX_DELAY);
      Serial1.println("Entering Task4");
      xSemaphoreTake( accMutex, portMAX_DELAY );
      global_velocity=local_velocity;
      global_position=local_position;
      stripe_count=local_stripe_count;
      xSemaphoreGive( accMutex );
      if (local_position - local_stripe_count > 150){
        xTaskNotifyGive(E);
      }
      else {
        //rrTimer3.end();
        xTimerStop(Timer3, portMAX_DELAY);
        rr_flag = 1;
        //rrTimer2.begin(Task7, 5); //DIS_BW_2_SENSORS/local_velocity
        //xTimerStart(Timer2, pdMS_TO_TICKS(DIS_BW_2_SENSORS/local_velocity));
        xTimerStart(Timer2, pdMS_TO_TICKS(5000));
      }
    }
}


void Task5(void* args){
  uint32_t local_position;
  uint32_t local_velocity;
  uint8_t local_stripe_count;
  for(;;){
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    Serial1.println("Entering Task5");
    xSemaphoreTake(accMutex, portMAX_DELAY);
    global_velocity=local_velocity;
    global_position=local_position;
    stripe_count=local_stripe_count;
    if(local_position - local_stripe_count > 150){
      xTaskNotifyGive(E);
    }
    else{
      //Notify the base station
      //rrTimer3.end();
      xTimerStop(Timer3, portMAX_DELAY);
      sensor1_error_counter+=1;
      if (sensor1_error_counter == 2) 
          xTaskNotifyGive(E);
        //rrTimer1.begin(Task3, 5); //DIS_BW_2_STRIPES/local_velocity
        //xTimerStart(Timer1, pdMS_TO_TICKS(DIS_BW_2_STRIPES/local_velocity));
        xTimerStart(Timer1, pdMS_TO_TICKS(1000));
        //Update pos, stripe_count
        if (local_stripe_count == 16){
          system_state = 5;
          sys_state_change.buf[0]=5;
            Can0.write(sys_state_change);
            xTaskNotifyGive(BPID);
        }
        rr_flag=0;
    }
  }
}



void Task6(void* args){
  //Notify Base Station of the Error: Sensor 1 missed a stripe
  double local_velocity;
    uint32_t local_position;
    uint8_t local_stripe_count;
  for(;;){
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
      //rrTimer1.end();
    Serial1.println("Entering Task6");
      xTimerStop(Timer1, portMAX_DELAY);
      sensor1_error_counter+=1;
      if (sensor1_error_counter == 2){
        xTaskNotifyGive(E);
      } 
      //rrTimer1.begin(Task3,5);//DIS_BW_2_STRIPES/local_velocity
      //xTimerStart(Timer1, pdMS_TO_TICKS(DIS_BW_2_STRIPES/local_velocity));
      xTimerStart(Timer1, pdMS_TO_TICKS(1000));
      //global_velocity=local_velocity;
      xSemaphoreTake(accMutex, portMAX_DELAY);
      global_position=local_position;
      stripe_count=local_stripe_count;
      xSemaphoreGive(accMutex);
      if (stripe_count == 16){
        system_state=5;
        sys_state_change.buf[0]=5;
        Can0.write(sys_state_change);
        xTaskNotifyGive(BPID);
      }
    //Update pos, stripe_count

  }
}


void Task7(void* args){
  double local_velocity;
    uint32_t local_position;
    uint8_t local_stripe_count;

  for(;;){
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    //  Notify Base Station
      rr_flag=0;
    //  Update pos, stripe count
      xSemaphoreTake( accMutex, portMAX_DELAY );
      //global_velocity=local_velocity;
      global_position=local_position;
      stripe_count=local_stripe_count;
      xSemaphoreGive( accMutex );
      if (local_stripe_count == 16){
        system_state=5;
        sys_state_change.buf[0]=5;
        Can0.write(sys_state_change);
        xTaskNotifyGive(BPID);
      }
      sensor2_error_counter+=1;
      if(sensor2_error_counter==2){
        xTaskNotifyGive(E);
      }
      //rrTimer1.begin(Task3, 5);//DIS_BW_2_STRIPES/local_velocity 
      //xTimerStart(Timer1, pdMS_TO_TICKS(DIS_BW_2_STRIPES/local_velocity));
      xTimerStart(Timer1, pdMS_TO_TICKS(1000));
  }
}

/*
-----------------------------------------------------------------------------------------------------------------
RR Sensor tasks end here
-----------------------------------------------------------------------------------------------------------------
*/


/*
-----------------------------------------------------------------------------------------------------------------
Task definitions end here. Initialisation Function definitions begin
-----------------------------------------------------------------------------------------------------------------
*/

void init_acc(){
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  #if DEBUG_MODE
    Display.println(F("Initializing I2C devices..."));
  #endif

  mpu.initialize();
// verify connection
  Display.println("Testing device connections...");
  if(mpu.testConnection()){
    Display.println("MPU6050 connection successful");
  }
  else{
    Display.println("MPU6050 connection failed");
    initialise_status=0;
  }
  // wait for ready
  // load and configure the DMP
  
  Display.println("Initializing DMP..."); 
  devStatus = mpu.dmpInitialize();
  
  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(XGyroOffset);
  mpu.setYGyroOffset(YGyroOffset);
  mpu.setZGyroOffset(ZGyroOffset);
  mpu.setZAccelOffset(ZAccelOffset);
  mpu.setYAccelOffset(YAccelOffset);
  mpu.setXAccelOffset(XAccelOffset); // 1688 factory default for my test chip
  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
      // turn on the DMP, now that it's ready
      Display.println("Enabling DMP...");
      mpu.setDMPEnabled(true);
      // enable Arduino interrupt detection
      Display.println("Enabling interrupt detection (Arduino external interrupt 0)...");
      
      mpuIntStatus = mpu.getIntStatus();
      // set our DMP Ready flag so the main loop() function knows it's okay to use it
      Display.println("DMP ready! Waiting for first interrupt...");
      dmpReady = true;
      // get expected DMP packet size for later comparison
      packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
      // ERROR!
      // 1 = initial memory load failed
      // 2 = DMP configuration updates failed
      // (if it's going to break, usually the code will be 1)
      Display.println("DMP Initialization failed (code ");
      Display.println(devStatus);
      initialise_status=0;
      }
}



void init_gyro(){
  if(!bno1.begin()){
    Display.println("BNO055 failed to initialise");
  }
  else
    Display.println("BNO055 initialised successfully");
}

void init_rr(){

// Initialising the RR sensor pins

  pinMode(RR_Pin1, INPUT);
  attachInterrupt(RR_Pin1, isrService1, CHANGE);
  pinMode(RR_Pin2, INPUT);
  attachInterrupt(RR_Pin2, isrService2, CHANGE);
  Display.println("Retroreflective sensors initialised successfully");
}

 void init_can(){
    Can0.begin(CAN_BAUD);
    Can0.attachObj(&canListener1);
    canListener1.attachGeneralHandler();
    sys_state_change.ext = 0;
    sys_state_change.id = 0x05; //5
    sys_state_change.len = 1;
    sys_state_ack.ext = 0;
    sys_state_ack.id = 0x09; //9
    sys_state_ack.len = 1;
    health_ack.ext = 0;
    health_ack.id = 0x01; //1
    health_ack.len = 1;
    lts_packet1.ext = 0;
    lts_packet1.id = 0x13; //19
    lts_packet1.len = 8;
    lts_packet2.ext = 0;
    lts_packet2.id = 0x14; //20
    lts_packet2.len = 8;
    attitude_acc.ext = 0;
    attitude_acc.id = 0x10; //16
    attitude_acc.len = 8;
    *attitude_acc.buf = global_yaw;
    *(attitude_acc.buf+16)=global_pitch;
    *(attitude_acc.buf+32)=global_roll;
    *(attitude_acc.buf+48)=global_acceleration;
    pos_vel.ext = 0;
    pos_vel.id = 0x11; //17
    pos_vel.len = 8;
    *(pos_vel.buf) = global_position;
    *(pos_vel.buf+32) = global_velocity;
    stripe_count_msg.ext = 0;
    stripe_count_msg.id = 0x12; //18
    stripe_count_msg.len = 1;
    stripe_count_msg.buf[0] = stripe_count;
   Display.println("Navigation: CAN initialised");
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
 }

void init_software_timer(){
  dataLogTimer = xTimerCreate("DataLog&CAN", pdMS_TO_TICKS(DataLogTimerPeriod), pdTRUE, 0, dataLogging);  
  if(dataLogTimer !=NULL){
    BaseType_t DataTimerStarted = xTimerStart(dataLogTimer,0);
    Display.println("Navigation: Software timer created successfully");
    if(DataTimerStarted==pdPASS){
      Display.println("Navigation: Software timer started successfully");
    }
    else{
      Display.println("Navigation: Unable to start software timer");
      initialise_status=0;
    }
  }
  else{
    Display.println("Navigation: Unable to create software timer");
  }
}

void posUpdate (void* args)
{
  while(1)
  {
    xSemaphoreTake(accMutex, portMAX_DELAY);
    global_position += 5;
    global_velocity += 1;
    Serial1.print("Global Position:");
    Serial1.println(global_position);
    Serial1.print("Global Velocity:");
    Serial1.println(global_velocity);
    xSemaphoreGive (accMutex);
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}


void init_lts(){

}


int main(void) {

/*
-----------------------------------------------------------------------------------------------------------------
Navigation software initialisation begins
-----------------------------------------------------------------------------------------------------------------
*/ 

  Display.begin(9600);
  Display.println("Starting initialisation");
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delayMicroseconds(1000000);
  digitalWrite(13, LOW);
  delayMicroseconds(1000000);

  // init_acc();
  // int init_gyro();
   init_rr();
   init_can();
  // init_sdcard();
  // init_software_timer();
  // init_lts();

  accMutex = xSemaphoreCreateMutex();
  grp=xEventGroupCreate();

  if(initialise_status!=1){
    Display.println("Navigation initialisation Failed. Can't proceed");
    while(1);
  }
  else{
    Display.println("Initialisation successful");
  }

/*
-----------------------------------------------------------------------------------------------------------------
Navigation software initialisation ends
-----------------------------------------------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------------------------------------------
Run the checklist
-----------------------------------------------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------------------------------------------
Create the task list 
-----------------------------------------------------------------------------------------------------------------
*/
  
  
  xTaskCreate(Emergency, "Emergency", configMINIMAL_STACK_SIZE, NULL, 4, &E);
  xTaskCreate(canSend, "CANS", configMINIMAL_STACK_SIZE, NULL, 2, &CANS);
  xTaskCreate(dataLogging, "DataL", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(brakingPID, "PID", configMINIMAL_STACK_SIZE, NULL, 1, &BPID);
  xTaskCreate(brakingStop, "BrStop", configMINIMAL_STACK_SIZE, NULL, 1, &BrStop);
 // xTaskCreate(gyro, "Gyro", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
 // xTaskCreate(lts, "LaserT", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
 //  xTaskCreate(accelerometer, "Acc", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
 //   xTaskCreate(retroSensorBothHit, "RR1", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
 //   xTaskCreate(retroSensorOneHit, "RR2", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
//    xTaskCreate(retroSensorOneMissTwoHit, "RR3", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
  // xTaskCreate(rrServicing1, "RR Timer1 Expire Service", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
  // xTaskCreate(rrServicing2, "RR Timer2 Expire Service", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
 // xTaskCreate(stateEstimate, "DeadR", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
   xTaskCreate(linearActuator, "LinAct", configMINIMAL_STACK_SIZE, NULL, 1, &LinAct); //Prty
   xTaskCreate(brakeRetraction, "brRetraction", configMINIMAL_STACK_SIZE, NULL, 1, &brRetraction);
// xTaskCreate(test, "Test", configMINIMAL_STACK_SIZE, NULL, 6, NULL);
// xTaskCreate(test1, "Test1", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    xTaskCreate(Task1, "Task1", configMINIMAL_STACK_SIZE, NULL, 1, &TASK1);
    xTaskCreate(Task2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1, &TASK2);
    xTaskCreate(Task3, "Task3", configMINIMAL_STACK_SIZE, NULL, 1, &TASK3);
    xTaskCreate(Task4, "Task4", configMINIMAL_STACK_SIZE, NULL, 1, &TASK4);
    xTaskCreate(Task5, "Task5", configMINIMAL_STACK_SIZE, NULL, 1, &TASK5); 
    xTaskCreate(Task6, "Task6", configMINIMAL_STACK_SIZE, NULL, 1, &TASK6);
    xTaskCreate(Task7, "Task7", configMINIMAL_STACK_SIZE, NULL, 1, &TASK7);
    xTaskCreate(posUpdate, "posUpdate", configMINIMAL_STACK_SIZE, NULL, 1, &PU);
    Timer1 = xTimerCreate("Timer1", pdMS_TO_TICKS(10000), pdFALSE, (void *)2, Task3);
    Timer2 = xTimerCreate("Timer2", pdMS_TO_TICKS(10000), pdFALSE, (void *)2, Task7);
    Timer3 = xTimerCreate("Timer3", pdMS_TO_TICKS(10000), pdFALSE, (void *)2, Task3);

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
