//----------------------------INSTrUCTIONS!!--------------------------------
// 1. FlexCAN.h -----> #define IRQ_PRIORITY 96
// 2. queue.c -----> Uncomment lines 1094 (configASSERT) if commented


// //Receive with no masks and distinct mailboxes

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
/*sensor libraries*/
#include <FlexCAN.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <i2c_t3.h>
#include <FlexCAN.h>


//Message IDs of packets to be sent
#define HLTHACK_ID 0x03 //3  (Health Ack)                  
#define SYS_STATE_PKT_ID 0x22 //34  (System state change)
#define SSACK_ID 0x42 //66  (System State ack)

//CAN Filters
#define FILTER_0 0x01    //Filter for Health Pkt from Comm node  (0x01)
#define FILTER_1 0x21    //Filter for System State change packets (0x21,0x23,0x24)
#define FILTER_2 0x23    //Filter for System State change packets (0x21,0x23,0x24)
#define FILTER_3 0x24    //Filter for System State change packets (0x21,0x23,0x24)  
/*define freeRTOS handlers*/
SemaphoreHandle_t accMutex;//semaphore handles to lock complete function execution
EventGroupHandle_t grp;//eventGroup handles for ISR and their related functions
SemaphoreHandle_t PKTHDLE0, PKTHDLE1, PKTHDLE2, PKTHDLE3, PKTHDLE4, PKTHDLE5, PKTHDLE6, PKTHDLE7, PKTHDLE8, PKTHDLE9;

/*define all global variables*/
CAN_message_t msg;//msg in the can send buffer
CAN_message_t msgRec0;//msg in the can Receive buffer
CAN_message_t msgRec1;//msg in the can Receive buffer
CAN_message_t msgRec2;//msg in the can Receive buffer
CAN_message_t msgRec3;//msg in the can Receive buffer
/*defining CAN packets*/
CAN_message_t sys_state_change;
CAN_message_t sys_state_ack;
CAN_message_t health_ack;

/*function definations*/
/*sensor functions*/
void CANReceive0(void *args);  //analyse data packet 0 from CAN
void CANReceive1(void *args);  //analyse data packet 1 from CAN
void CANReceive2(void *args);  //analyse data packet 1 from CAN
void CANReceive3(void *args);  //analyse data packet 1 from CAN


/*CAN listener class*/
class CanListener1 : public CANListener 
{
public:
  bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

CanListener1 canListener1;

bool CanListener1::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
  //msgRec=frame;
   switch(mailbox){
      case 0: {
        xSemaphoreGiveFromISR(PKTHDLE0, pdFALSE);
        msgRec0 = frame;
        break;
      }      
      case 1: {
        xSemaphoreGiveFromISR(PKTHDLE1, pdFALSE);
        msgRec1 = frame;
        break;
      }
      case 2: {
        xSemaphoreGiveFromISR(PKTHDLE2, pdFALSE);
        msgRec2 = frame;
        break;
      }      
      case 3: {
        xSemaphoreGiveFromISR(PKTHDLE3, pdFALSE);
        msgRec3 = frame;
        break;
      }
    }
}


//Receiving Health Packet from Comm Node
void CANReceive0(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE0, portMAX_DELAY);  
    //taskDISABLE_INTERRUPTS();
    Serial1.print("health pkt received : ");
    Serial1.println(msgRec0.buf[0]);
          GPIOC_PTOR = 1 << 5;
    Can0.write(health_ack);   //Send Health Packet Acknowledge
    //taskENABLE_INTERRUPTS();
  }
}

//Receiving System State Change packet
void CANReceive1(void *args){
 	uint8_t counter1 = 0;
 	//uint8_t counter2 = 0;
 	//uint8_t counter3 = 0;
  	for (;;){
    xSemaphoreTake(PKTHDLE1, portMAX_DELAY);
    //taskDISABLE_INTERRUPTS();
    //if (msgRec1.id == 0x21){
      Serial1.print("New system state : 2");
      //taskDISABLE_INTERRUPTS();
      sys_state_ack.buf[0]=1;
      sys_state_ack.buf[1]=counter1++;
      Can0.write(sys_state_ack);
      //taskENABLE_INTERRUPTS();
    //}
    }
 }

void CANReceive2(void *args){
 	//uint8_t counter1 = 0;
 	uint8_t counter2 = 0;
 	//uint8_t counter3 = 0;
  	for (;;){
    xSemaphoreTake(PKTHDLE2, portMAX_DELAY);
    //taskDISABLE_INTERRUPTS();
    //if (msgRec1.id == 0x23){
      Serial1.print("New system state : 3");
      //taskDISABLE_INTERRUPTS();
      sys_state_ack.buf[0]=3;
      sys_state_ack.buf[1]=counter2++;
      Can0.write(sys_state_ack);
      //taskENABLE_INTERRUPTS();
    //}
  }
}


void CANReceive3(void *args){
 	// uint8_t counter1 = 0;
 	// uint8_t counter2 = 0;
 	uint8_t counter3 = 0;
  	for (;;){
    xSemaphoreTake(PKTHDLE3, portMAX_DELAY);
    //taskDISABLE_INTERRUPTS();
    //if (msgRec1.id == 0x24){
      Serial1.print("New system state : 4");
      //taskDISABLE_INTERRUPTS();
      sys_state_ack.buf[0]=4;
      sys_state_ack.buf[1]=counter3++;
      Can0.write(sys_state_ack);
      //taskENABLE_INTERRUPTS();
    //}
  }
}

//setup the pinModes and create tasks

int main(){
    //Serial Setup
  Serial1.begin(9600);
  Serial1.println ("Hello World!");
  Can0.begin(1000000);
  Can0.attachObj(&canListener1);
  canListener1.attachMBHandler(0);
  canListener1.attachMBHandler(1);
  canListener1.attachMBHandler(2);
  canListener1.attachMBHandler(3);
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
  //sys_state_ack.buf[0]=0;
  
  CAN_filter_t filter0;
  CAN_filter_t filter1;
  CAN_filter_t filter2;
  CAN_filter_t filter3;
  // Framehandler invoked only for id 560
  filter0.flags.extended=0;
  filter0.flags.remote=0;
  filter0.flags.reserved=0;
  filter0.id=FILTER_0;
  Can0.setFilter(filter0,0);
  Can0.setMask(0x1FFFFFFF,0);   

  filter1.flags.extended=0;
  filter1.flags.remote=0;
  filter1.flags.reserved=0;
  filter1.id=FILTER_1;
  Can0.setFilter(filter1,1);
  Can0.setMask(0x1FFFFFFF,1);   

  filter2.flags.extended=0;
  filter2.flags.remote=0;
  filter2.flags.reserved=0;
  filter2.id=FILTER_2;
  Can0.setFilter(filter2,2);
  Can0.setMask(0x1FFFFFFF,2);   

  filter3.flags.extended=0;
  filter3.flags.remote=0;
  filter3.flags.reserved=0;
  filter3.id=FILTER_3;
  Can0.setFilter(filter3,3);
  Can0.setMask(0x1FFFFFFF,3);
  
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  delayMicroseconds(1000000);
  digitalWrite(13,LOW);

  
  PKTHDLE0 = xSemaphoreCreateBinary();   //Mutexes to redirect control from FrameHandler to CANRcv tasks
  PKTHDLE1 = xSemaphoreCreateBinary();    
  PKTHDLE2 = xSemaphoreCreateBinary();   //Mutexes to redirect control from FrameHandler to CANRcv tasks
  PKTHDLE3 = xSemaphoreCreateBinary();
  xTaskCreate(CANReceive0, "CANReceive0", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive1, "CANReceive1", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive2, "CANReceive2", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive3, "CANReceive3", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  vTaskStartScheduler();
  for(;;);  
  return 0;
}

// //-----------------Receive with masks and 1 mailbox for SystemState

// /*headers for freeRTOS*/
// #include <kinetis.h>
// #include <stdlib.h>
// #include <FreeRTOS.h>
// #include <task.h>
// #include <Arduino.h>
// #include <queue.h>
// #include <timers.h>
// #include <semphr.h>
// #include <event_groups.h>
// /*sensor libraries*/
// #include <FlexCAN.h>
// #include <SPI.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BMP280.h>
// #include <i2c_t3.h>
// #include <FlexCAN.h>




// //Message IDs of packets to be sent
// #define HLTHACK_ID 0x03 //3  (Health Ack)                  
// #define SYS_STATE_PKT_ID 0x22 //34  (System state change)
// #define SSACK_ID 0x42 //66  (System State ack)

// //CAN Filters
// #define FILTER_0 0x01    //Filter for Health Pkt from Comm node  (0x01)
// #define FILTER_1 0x20    //Filter for System State change packets (0x21,0x23,0x24)
// /*define freeRTOS handlers*/
// SemaphoreHandle_t accMutex;//semaphore handles to lock complete function execution
// EventGroupHandle_t grp;//eventGroup handles for ISR and their related functions
// SemaphoreHandle_t PKTHDLE0, PKTHDLE1, PKTHDLE2, PKTHDLE3, PKTHDLE4, PKTHDLE5, PKTHDLE6, PKTHDLE7, PKTHDLE8, PKTHDLE9;

// /*define all global variables*/
// CAN_message_t msg;//msg in the can send buffer
// CAN_message_t msgRec0;//msg in the can Receive buffer
// CAN_message_t msgRec1;//msg in the can Receive buffer
// // CAN_message_t msgRec2;//msg in the can Receive buffer
// // CAN_message_t msgRec3;//msg in the can Receive buffer
// /*defining CAN packets*/
// CAN_message_t sys_state_change;
// CAN_message_t sys_state_ack;
// CAN_message_t health_ack;

// /*function definations*/
// /*sensor functions*/
// void CANReceive0(void *args);  //analyse data packet 0 from CAN
// void CANReceive1(void *args);  //analyse data packet 1 from CAN
// // void CANReceive2(void *args);  //analyse data packet 1 from CAN
// // void CANReceive3(void *args);  //analyse data packet 1 from CAN


// /*CAN listener class*/
// class CanListener1 : public CANListener 
// {
// public:
//   bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
// };

// CanListener1 canListener1;

// bool CanListener1::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
// {
//   //msgRec=frame;
//    switch(mailbox){
//       case 0: {
//         xSemaphoreGiveFromISR(PKTHDLE0, pdFALSE);
//         msgRec0 = frame;
//         break;
//       }      
//       case 1: {
//         xSemaphoreGiveFromISR(PKTHDLE1, pdFALSE);
//         msgRec1 = frame;
//         break;
//       }
//     }
// }


// //Receiving Health Packet from Comm Node
// void CANReceive0(void *args){
//   for (;;){
//     xSemaphoreTake(PKTHDLE0, portMAX_DELAY);  
//     //taskDISABLE_INTERRUPTS();
//     Serial1.print("health pkt received : ");
//     Serial1.println(msgRec0.buf[0]);
//           GPIOC_PTOR = 1 << 5;
//     Can0.write(health_ack);   //Send Health Packet Acknowledge
//     //taskENABLE_INTERRUPTS();
//   }
// }

// //Receiving System State Change packet
// void CANReceive1(void *args){
//  	uint8_t counter1 = 0;
//  	uint8_t counter2 = 0;
//  	uint8_t counter3 = 0;
//   	for (;;){
// 	    xSemaphoreTake(PKTHDLE1, portMAX_DELAY);
// 	    if (msgRec1.id == 0x21){
// 	      Serial1.print("New system state : 2");
// 	      sys_state_ack.buf[0]=1;
// 	      sys_state_ack.buf[1]=counter1++;
// 	      Can0.write(sys_state_ack);
// 	    }
// 	    if (msgRec1.id == 0x23){
// 	      Serial1.print("New system state : 3");
// 	      sys_state_ack.buf[0]=3;
// 	      sys_state_ack.buf[1]=counter2++;
// 	      Can0.write(sys_state_ack);
// 	    }
// 	    if (msgRec1.id == 0x24){
// 	      Serial1.print("New system state : 4");
// 	      sys_state_ack.buf[0]=4;
// 	      sys_state_ack.buf[1]=counter3++;
// 	      Can0.write(sys_state_ack);
// 	  	}
//     }
//  }

// //setup the pinModes and create tasks

// int main(){
//     //Serial Setup
//   Serial1.begin(9600);
//   Serial1.println ("Hello World!");
//   Can0.begin(1000000);
//   Can0.attachObj(&canListener1);
//   canListener1.attachMBHandler(0);
//   canListener1.attachMBHandler(1);
//   //initializing CAN message packets to be sent from this node 
//   health_ack.ext = 0;
//   health_ack.id = HLTHACK_ID;
//   health_ack.len = 1;
//   health_ack.buf[0]=0;
  
//   sys_state_change.ext = 0;
//   sys_state_change.id = SYS_STATE_PKT_ID;
//   sys_state_change.len = 1;
  
//   sys_state_ack.ext = 0;
//   sys_state_ack.id = SSACK_ID;
//   sys_state_ack.len = 2;
//   //sys_state_ack.buf[0]=0;
  
//   CAN_filter_t filter0;
//   CAN_filter_t filter1;
//   // Framehandler invoked only for id 560
//   filter0.flags.extended=0;
//   filter0.flags.remote=0;
//   filter0.flags.reserved=0;
//   filter0.id=FILTER_0;
//   Can0.setFilter(filter0,0);
//   Can0.setMask(0x1FFFFFFF,0);   

//   filter1.flags.extended=0;
//   filter1.flags.remote=0;
//   filter1.flags.reserved=0;
//   filter1.id=FILTER_1;
//   Can0.setFilter(filter1,1);
//   Can0.setMask(0xFFF0,1);   

//   pinMode(13,OUTPUT);
//   digitalWrite(13,HIGH);
//   delayMicroseconds(1000000);
//   digitalWrite(13,LOW);

  
//   PKTHDLE0 = xSemaphoreCreateBinary();   //Mutexes to redirect control from FrameHandler to CANRcv tasks
//   PKTHDLE1 = xSemaphoreCreateBinary();    
//   xTaskCreate(CANReceive0, "CANReceive0", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
//   xTaskCreate(CANReceive1, "CANReceive1", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
//   vTaskStartScheduler();
//   for(;;);  
//   return 0;
// }



//Send
// #include <Arduino.h>
// #include <FlexCAN.h>
// #include <FreeRTOS.h>
// #include <task.h>

// void HealthSend (void* args);
// void SysStateFrmNav (void* args);
// void SysStateFrmCtrl (void* args);
// void SysStateFrmComm (void* args);

// class CanListener1 : public CANListener 
// {
// public:
//   bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
// };

// CanListener1 canListener1;
// CAN_message_t Health_msg, SystemState, SystemStateAck, BrakingAct, LSD, LinAct, Clutch;


// bool CanListener1::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
// {
//   digitalWrite(13, HIGH);
//   delayMicroseconds(100000);
//   digitalWrite(13, LOW);
//   Serial1.println(frame.id);
//   for (int i=0; i<8; i++)
//     Serial1.println(frame.buf[i]);  
//   return true;
// }

// void HealthSend (void* args){
//   for(;;){
//     Can0.write(Health_msg);
//     vTaskDelay(pdMS_TO_TICKS(50));
//   }
// }

// void SysStateFrmNav (void* args){
//   for(;;){
//     SystemState.id = 0x21;
//     SystemState.buf[0] = 2;
//     Can0.write(SystemState);
//     vTaskDelay(pdMS_TO_TICKS(50));
//   }
// }

// void SysStateFrmCtrl (void* args){
//   for(;;){
//     SystemState.id = 0x23;
//     SystemState.buf[0] = 3;
//     Can0.write(SystemState);
//     vTaskDelay(pdMS_TO_TICKS(50)); 
//   }
// }

// void SysStateFrmComm (void* args){
//   for(;;){
//     SystemState.id = 0x24;
//     SystemState.buf[0] = 4;
//     Can0.write(SystemState);
//     vTaskDelay(pdMS_TO_TICKS(50));
//   }
// }


// int main()
// {
//   Health_msg.ext = 0;
//   Health_msg.len = 1;
//   Health_msg.id = 0x01;
//   Health_msg.buf[0] = 0;

//   SystemState.ext = 0;
//   SystemState.len = 1;

//   SystemStateAck.ext = 0;
//   SystemStateAck.len = 1;
//   SystemStateAck.buf[0] = 0;

//   BrakingAct.ext = 0;
//   BrakingAct.len = 8;

//   LSD.ext = 0;
//   LSD.len = 8;

//   LinAct.ext = 0;
//   LinAct.len = 8;

//   Clutch.ext = 0;
//   Clutch.len = 1;
//   Clutch.id = 0x81;    
  
//   Serial1.begin(9600);
//   pinMode (13, OUTPUT);
//   digitalWrite(13, HIGH);
//   delayMicroseconds(100000);
//   digitalWrite(13, LOW);
//   Can0.begin(1000000);
//   Can0.attachObj(&canListener1);
//   canListener1.attachGeneralHandler();

//   xTaskCreate(HealthSend, "HS", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
//   xTaskCreate(SysStateFrmNav, "SSNav", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
//   xTaskCreate(SysStateFrmCtrl, "SSCtrl", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
//   xTaskCreate(SysStateFrmComm, "SSComm", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  
//   vTaskStartScheduler();

//   for(;;)
//   return 0;
// }

