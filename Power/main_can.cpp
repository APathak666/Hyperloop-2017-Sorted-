
/*headers for freeRTOS*/
#include <kinetis.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <Arduino.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
#include <event_groups.h>
#include <FlexCAN.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <i2c_t3.h>
#include <FlexCAN.h>


/*general variables*/
#define Serial_BAUD 9600
/*sensor pins*/

// /*CAN settings*/
// #define CAN_ID1 0x01  //CAN ID for packet 1 
// #define CAN_ID2 0x02  //CAN ID for packet 2
// #define CAN_ID3 0x03  //CAN ID for packet 3
// #define TEST_ID 0x11  //CAN ID for functional tests
// #define CAN_FILTER_NAV 0x100 //CAN FILTER ID for data from Nav node
// #define CAN_FILTER_COMM 0x100 //CAN FILTER ID for data from Comm node 

//Message IDs of packets to be sent
#define HLTHACK_ID 0x03 //3  (Health Ack)                  
#define SYS_STATE_PKT_ID 0x22 //34  (System state change)
#define SSACK_ID 0x42 //66  (System State ack)
#define CANB1_ID 0xC1 //193 (CAN Broadcast Pkt1 : Voltagex4, Amb Temp, Amb Press)
#define CANB2_ID 0xC2 //194 (CAN Broadcast Pkt2 : Currentx4)
#define CANB3_ID 0xC3 //195 (CAN Broadcast Pkt3 : Tempx4)

//CAN Filters
#define FILTER_0 0x00    //Filter for Health Pkt from Comm node  (0x01)
#define FILTER_1 0x20    //Filter for System State change packets (0x21,0x23,0x24)
#define FILTER_2 0x40    //Filter for System State Ack packets (0x41,0x43,0x44) 
#define FILTER_3 0x61    //Filter for Braking Actuation Pkt from Nav: online (0x61)
#define FILTER_4 0x62    //Filter for LSD RPM Packet from Nav: online (0x62)
#define FILTER_5 0x63    //Filter for Linear Actuator Pkt from Nav: online (0x63)
#define FILTER_6 0x81    //Filter for Clutch Engage Pkt from Comm: offline (0x81)
#define FILTER_7 0x82    //Filter for Braking Actuation Pkt from Comm: offline (0x82)
#define FILTER_8 0x83    //Filter for LSD RPM Packet from Comm: offline (0x83)
#define FILTER_9 0x84    //Filter for Linear Actuator Pkt from Comm: offline (0x84)

SemaphoreHandle_t PKTHDLE[10];
//0, PKTHDLE1, PKTHDLE2, PKTHDLE3, PKTHDLE4, PKTHDLE5, PKTHDLE6, PKTHDLE7, PKTHDLE8, PKTHDLE9;
TaskHandle_t voltFunc=NULL;

/*define all global variables*/
volatile int system_state=0;//System state

CAN_message_t msg;//msg in the can send buffer
CAN_message_t msgRec[10];//msg in the can Receive buffer

/*defining CAN packets*/
CAN_message_t sys_state_change;
CAN_message_t sys_state_ack;
CAN_message_t health_ack;
CAN_message_t volt4Temp1Press1; //Packet1: Voltages of 4 battery packs, 1 Ambient Temperature & 1 Ambient Pressure 
CAN_message_t current4; //Packet2: Current from 4 battery packs
CAN_message_t temp4;  //Packet3: Temp from 4 battery packs

void CANReceive0(void *args);  //analyse data packet 0 from CAN
void CANReceive1(void *args);  //analyse data packet 1 from CAN
void CANReceive2(void *args);  //analyse SystemState packet from CAN
void CANReceive3(void *args);  //analyse data packet 3 from CAN
void CANReceive4(void *args);  //analyse data packet 4 from CAN
void CANReceive5(void *args);  //analyse data packet 5 from CAN
void CANReceive6(void *args);  //analyse data packet 6 from CAN
void CANReceive7(void *args);  //analyse data packet 7 from CAN
void CANReceive8(void *args);  //analyse data packet 8 from CAN
void CANReceive9(void *args);  //analyse data packet 9 from CAN



/*CAN listener class*/
class CanListener1 : public CANListener 
{
public:
  bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

CanListener1 canListener1;

bool CanListener1::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
	msgRec[mailbox] = frame;
    xSemaphoreGiveFromISR(PKTHDLE[mailbox], pdFALSE);
    return true;
}

//Receiving Health Packets 
void CANReceive0(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[0], portMAX_DELAY);  
    taskDISABLE_INTERRUPTS();
    Serial1.print("health pkt received : ");
    Serial1.println(msgRec[0].buf[0]);
    Can0.write(health_ack);   //Send Health Packet Acknowledge
    taskENABLE_INTERRUPTS();
  }
}

//Receiving System State Change packet
void CANReceive1(void *args){
  uint8_t counter1=0;
  uint8_t counter2=0;
  uint8_t counter3=0;
  for (;;){
    xSemaphoreTake(PKTHDLE[1], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    Serial1.print("New system state : ");
    Serial1.println(msgRec1.buf[0]);
    if (msgRec[1].id==0x21){
      sys_state_ack.buf[0] = 1;
      sys_state_ack.buf[1] = counter1++;
      Can0.write(sys_state_ack);
    }
    if (msgRec[1].id==0x23){
      sys_state_ack.buf[0] = 3;
      sys_state_ack.buf[1] = counter2++;
      Can0.write(sys_state_ack);
    }
    if (msgRec[1].id==0x24){
      sys_state_ack.buf[0] = 4;
      sys_state_ack.buf[1] = counter3++;
      Can0.write(sys_state_ack);
    }
    taskENABLE_INTERRUPTS();  //Send system state change acknowledge
  }
}

//Receiving System state change Ack
void CANReceive2(void* args){
  for (;;){
    xSemaphoreTake(PKTHDLE[2], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    if (msgRec[2].id==0x41)
      Serial1.print("Received system state change ack from Nav Node");
    if (msgRec[2].id==0x43)
      Serial1.print("Received system state change ack from Control Node");
    if (msgRec[2].id==0x44)
      Serial1.print("Received system state change ack from Comm Node");      
  }
}

//Receiving Braking Actuation from Nav node when Online
void CANReceive3(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[3], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    Serial1.print("Value of Braking Actuation : ");
    for (int i=0; i<8 ;i++)
      Serial1.println(msgRec[3].buf[i]);
    taskENABLE_INTERRUPTS();
    
  }
}


//Receiving LSD actuation from Nav node when Online
void CANReceive4(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[4], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    Serial1.print("Value of LSD RPM : ");
    for (int i=0; i<8 ;i++)
      Serial1.println(msgRec[4].buf[i]);
    taskENABLE_INTERRUPTS();
  }
}

//Receiving Linear actuator pkt from Nav node when Online
void CANReceive5(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[5], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    Serial1.print("Value of Linear Actuation : ");
    for (int i=0; i<8 ;i++)
      Serial1.println(msgRec[5].buf[i]);
    taskENABLE_INTERRUPTS();
  }
}

//Receiving signal to Engage/Disengage Clutch
void CANReceive6(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[6], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    if (msgRec[6].buf[0]==0)
      Serial1.print("Clutch Engaged");
    if (msgRec[6].buf[0]==1)
      Serial1.println("Clutch Disengaged");
    taskENABLE_INTERRUPTS();
  }
}

//Receiving Braking Actuation from Comm node when Offline
void CANReceive7(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[7], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    /*
    Serial1.print("Value of Braking Actuation : ");
    for (int i=0; i<8 ;i++)
      Serial1.println(msgRec[7].buf[i]);
    */
    //Add stepper.cpp ka code
    taskENABLE_INTERRUPTS();
  }
}


//Receiving LSD actuation from Comm node when Offline
void CANReceive8(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[8], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    Serial1.print("Value of LSD RPM : ");
    for (int i=0; i<8 ;i++)
      Serial1.println(msgRec[8].buf[i]);
    taskENABLE_INTERRUPTS();
  }
}

//Receiving Linear actuator pkt from Comm node when Offline
void CANReceive9(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[9], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    Serial1.print("Value of Linear Actuation : ");
    for (int i=0; i<8 ;i++)
      Serial1.println(msgRec[9].buf[i]);
    taskENABLE_INTERRUPTS();
  }
}



int main(){
    //Serial Setup
  Serial1.begin(Serial_BAUD);
  Serial1.println ("Hello World!");
//  Serial4.begin(Serial_BAUD);
//  Serial4.begin(Serial_BAUD);
  //CAN Setup
  Can0.begin(1000000);
  Can0.attachObj(&canListener1);
  //canListener1.attachGeneralHandler();
  canListener1.attachMBHandler(0);
  canListener1.attachMBHandler(1);
  canListener1.attachMBHandler(2);
  canListener1.attachMBHandler(3);
  canListener1.attachMBHandler(4);
  canListener1.attachMBHandler(5);
  canListener1.attachMBHandler(6);
  canListener1.attachMBHandler(7);
  canListener1.attachMBHandler(8);
  canListener1.attachMBHandler(9);

  //initializing CAN message packets to be sent from this node 
  health_ack.ext = 0;
  health_ack.id = HLTHACK_ID;
  health_ack.len = 1;
  health_ack.buf[0]=0;
  
  sys_state_change.ext = 0;
  sys_state_change.id = SYS_STATE_PKT_ID;
  sys_state_change.len = 1;
  
  sys_state_ack.ext = 0;
  sys_state_ack.id = SSACK_ID;
  sys_state_ack.len = 2;
  sys_state_ack.buf[0]=0;
  
  volt4Temp1Press1.ext = 0;
  volt4Temp1Press1.id = CANB1_ID;
  volt4Temp1Press1.len = 8;

  current4.ext = 0;
  current4.id = CANB2_ID;
  current4.len = 8;

  temp4.ext = 0;
  temp4.id = CANB3_ID;
  temp4.len = 8;  

  CAN_filter_t filter0;
  CAN_filter_t filter1;
  CAN_filter_t filter2;
  CAN_filter_t filter3;
  CAN_filter_t filter4;
  CAN_filter_t filter5;
  CAN_filter_t filter6;
  CAN_filter_t filter7;
  CAN_filter_t filter8;
  CAN_filter_t filter9; 
  
  // Framehandler invoked only for id 560
  filter0.flags.extended=0;
  filter0.flags.remote=0;
  filter0.flags.reserved=0;
  filter0.id=FILTER_0;
  Can0.setFilter(filter0,0);
  Can0.setMask(0x1FFFFFF8,0);

  filter1.flags.extended=0;
  filter1.flags.remote=0;
  filter1.flags.reserved=0;
  filter1.id=FILTER_1;
  Can0.setFilter(filter1,1);
  Can0.setMask(0x1FFFFFF8,1);   

  filter2.flags.extended=0;
  filter2.flags.remote=0;
  filter2.flags.reserved=0;
  filter2.id=FILTER_2;
  Can0.setFilter(filter2,2);
  Can0.setMask(0x1FFFFFF8,2);

  filter3.flags.extended=0;
  filter3.flags.remote=0;
  filter3.flags.reserved=0;
  filter3.id=FILTER_3;
  Can0.setFilter(filter3,3);
  Can0.setMask(0x1FFFFFFF,3);

  filter4.flags.extended=0;
  filter4.flags.remote=0;
  filter4.flags.reserved=0;
  filter4.id=FILTER_4;
  Can0.setFilter(filter4,4);
  Can0.setMask(0x1FFFFFFF,4);

  filter5.flags.extended=0;
  filter5.flags.remote=0;
  filter5.flags.reserved=0;
  filter5.id=FILTER_5;
  Can0.setFilter(filter5,5);
  Can0.setMask(0x1FFFFFFF,5);
  
  filter6.flags.extended=0;
  filter6.flags.remote=0;
  filter6.flags.reserved=0;
  filter6.id=FILTER_6;
  Can0.setFilter(filter6,6);
  Can0.setMask(0x1FFFFFFF,6);

  filter7.flags.extended=0;
  filter7.flags.remote=0;
  filter7.flags.reserved=0;
  filter7.id=FILTER_7;
  Can0.setFilter(filter7,7);
  Can0.setMask(0x1FFFFFFF,7);

  filter8.flags.extended=0;
  filter8.flags.remote=0;
  filter8.flags.reserved=0;
  filter8.id=FILTER_8;
  Can0.setFilter(filter8,8);
  Can0.setMask(0x1FFFFFFF,8);

  filter9.flags.extended=0;
  filter9.flags.remote=0;
  filter9.flags.reserved=0;
  filter9.id=FILTER_9;
  Can0.setFilter(filter9,9);
  Can0.setMask(0x1FFFFFFF,9); 

  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  delayMicroseconds(1000000);
  digitalWrite(13,LOW);

//  stepper1.setSpeed(STEPPER_SPEED);

//  attachInterrupt(VOLTAGEIN,voltageISR,CHANGE);

  PKTHDLE[0] = xSemaphoreCreateMutex();   //Mutexes to redirect control from FrameHandler to CANRcv tasks
  PKTHDLE[1] = xSemaphoreCreateMutex();    
  PKTHDLE[2] = xSemaphoreCreateMutex();
  PKTHDLE[3] = xSemaphoreCreateMutex();
  PKTHDLE[4] = xSemaphoreCreateMutex();
  PKTHDLE[5] = xSemaphoreCreateMutex();
  PKTHDLE[6] = xSemaphoreCreateMutex();
  PKTHDLE[7] = xSemaphoreCreateMutex();
  PKTHDLE[8] = xSemaphoreCreateMutex();
  PKTHDLE[9] = xSemaphoreCreateMutex();

  xTaskCreate(CANReceive0, "CANReceive0", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive1, "CANReceive1", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive2, "CANReceive2", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive3, "CANReceive3", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive4, "CANReceive4", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive5, "CANReceive5", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive6, "CANReceive6", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive7, "CANReceive7", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive8, "CANReceive8", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive9, "CANReceive9", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  
  vTaskStartScheduler();
  for(;;);  
  return 0;
}




