// //--------------Testing CAN Receive of Power Node
#include <Arduino.h>
#include <FlexCAN.h>
#include <FreeRTOS.h>
#include <task.h>

void HealthSendFrmNav (void* args);
void HealthSendFrmCtrl (void* args);
void HealthSendFrmComm (void* args);
void SysStateFrmNav (void* args);
void SysStateFrmCtrl (void* args);
void SysStateFrmComm (void* args);
void SysStateAckFrmNav (void* args);
void SysStateAckFrmCtrl (void* ags);
void SysStateAckFrmComm (void* args);
void BrakingOnline (void* args);
void LSDOnline (void* args);
void LinActuatorOnline (void* args);
void BrakingOffline (void* args);
void LSDOffline (void* args);
void LinActuatorOffline (void* args);
void ClutchOffline (void* args);
void EmBraking (void* args);

TickType_t xLastWakeTimeBr, xLastWakeTimeLSD, xLastWakeTimeBr2, xLastWakeTimeLSD2;

class CanListener1 : public CANListener 
{
public:
  bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

CanListener1 canListener1;
CAN_message_t Health_msg, SystemState, SystemStateAck, BrakingAct, LSD, LinAct, Clutch;


bool CanListener1::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
  digitalWrite(13, HIGH);
  delayMicroseconds(100000);
  digitalWrite(13, LOW);
  Serial1.println(frame.id);
  for (int i=0; i<8; i++)
    Serial1.println(frame.buf[i]);  
  return true;
}

void HealthSendFrmNav (void* args){
  
  for(;;){
    Health_msg.id = 0x01;
  Health_msg.buf[0] = 1;
    Can0.write(Health_msg);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void HealthSendFrmCtrl (void* args){
  
  for(;;){
    Health_msg.id = 0x02;
  Health_msg.buf[0] = 2;
    Can0.write(Health_msg);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void HealthSendFrmComm (void* args){
  
  for(;;){
    Health_msg.id = 0x03;
  Health_msg.buf[0] = 3;
    Can0.write(Health_msg);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void SysStateFrmNav (void* args){
  for(;;){
    SystemState.id = 0x21;
    SystemState.buf[0] = 2;
    Can0.write(SystemState);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void SysStateFrmCtrl (void* args){
  for(;;){
    SystemState.id = 0x23;
    SystemState.buf[0] = 3;
    Can0.write(SystemState);
    vTaskDelay(pdMS_TO_TICKS(700)); 
  }
}

void SysStateFrmComm (void* args){
  for(;;){
    SystemState.id = 0x24;
    SystemState.buf[0] = 4;
    Can0.write(SystemState);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void SysStateAckFrmNav (void* args){
  for(;;){
    SystemStateAck.id = 0x41;
    Can0.write(SystemStateAck);
    vTaskDelay(pdMS_TO_TICKS(900));
  }
}

void SysStateAckFrmCtrl (void* args){
 for(;;){
    SystemStateAck.id = 0x43;
    Can0.write(SystemStateAck);
    vTaskDelay(pdMS_TO_TICKS(2000));
  } 
}

void SysStateAckFrmComm (void* args){
  for(;;){
    SystemStateAck.id = 0x44;
    Can0.write(SystemStateAck);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void BrakingOnline (void* args){
  xLastWakeTimeBr = xTaskGetTickCount();
  for(;;){
    BrakingAct.id = 0x71;
    *((uint16_t*) BrakingAct.buf) = 1670;
    Can0.write(BrakingAct);
    vTaskDelayUntil(&xLastWakeTimeBr, pdMS_TO_TICKS(500));
    
    BrakingAct.id = 0x71;
    *((uint16_t*) BrakingAct.buf) = 2000;
    Can0.write(BrakingAct);
    vTaskDelayUntil(&xLastWakeTimeBr, pdMS_TO_TICKS(500));
    
    BrakingAct.id = 0x71;
    *((uint16_t*) BrakingAct.buf) = 2500;
    Can0.write(BrakingAct);
    vTaskDelayUntil(&xLastWakeTimeBr, pdMS_TO_TICKS(500));
    
    BrakingAct.id = 0x71;
    *((uint16_t*) BrakingAct.buf) = 3000;
    Can0.write(BrakingAct);
    vTaskDelayUntil(&xLastWakeTimeBr, pdMS_TO_TICKS(500));
    
    BrakingAct.id = 0x71;
    *((uint16_t*) BrakingAct.buf) = 3300;
    Can0.write(BrakingAct);
    vTaskDelayUntil(&xLastWakeTimeBr, pdMS_TO_TICKS(500));
  }
}

void LSDOnline (void* args){
  xLastWakeTimeLSD = xTaskGetTickCount();
  for(;;){
    LSD.id = 0x72;
    LSD.buf[0] =  34;
    Can0.write(BrakingAct);
    vTaskDelayUntil(xLastWakeTimeLSD, pdMS_TO_TICKS(5000));
    LSD.id = 0x72;
    LSD.buf[0] =  0;
    Can0.write(LSD);
    vTaskDelayUntil(&xLastWakeTimeLSD, pdMS_TO_TICKS(5000));
  }
}


void LinActuatorOnline (void* args){
  for(;;){
    LinAct.id = 0x73;
    LinAct.buf[0] =  0x01;
    Can0.write(LinAct);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void BrakingOffline (void* args){
  xLastWakeTimeBr2 = xTaskGetTickCount();
  for(;;){
    BrakingAct.id = 0x82;
    BrakingAct.buf[0] =  25;
    Can0.write(BrakingAct);
    vTaskDelayUntil(&xLastWakeTimeBr2, pdMS_TO_TICKS(700));
    BrakingAct.id = 0x82;
    BrakingAct.buf[0] =  16;
    Can0.write(BrakingAct);
    vTaskDelayUntil(&xLastWakeTimeBr2, pdMS_TO_TICKS(700));
  }
}

void LSDOffline (void* args){
  xLastWakeTimeLSD2 = xTaskGetTickCount();
  for(;;){
    LSD.id = 0x83;
    LSD.buf[0] =  45;
    Can0.write(LSD);
    vTaskDelayUntil(&xLastWakeTimeLSD2, pdMS_TO_TICKS(5000));
    LSD.id = 0x83;
    LSD.buf[0] =  0;
    Can0.write(LSD);
    vTaskDelayUntil(&xLastWakeTimeLSD2, pdMS_TO_TICKS(5000));
  }
}

void LinActuatorOffline (void* args){
  for(;;){
    LinAct.id = 0x84;
    LinAct.buf[0] =  0x00;
    Can0.write(LinAct);
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void ClutchOffline (void* args){
  for(;;){
    Clutch.buf[0]=1;
    Can0.write(Clutch);
    vTaskDelay(pdMS_TO_TICKS(5000));
    Clutch.buf[0]=0;
    Can0.write(Clutch);
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

int main()
{
  Health_msg.ext = 0;
  Health_msg.len = 1;
  Health_msg.id = 0x01;
  Health_msg.buf[0] = 0;

  SystemState.ext = 0;
  SystemState.len = 1;

  SystemStateAck.ext = 0;
  SystemStateAck.len = 1;
  SystemStateAck.buf[0] = 0;

  BrakingAct.ext = 0;
  BrakingAct.len = 2;

  LSD.ext = 0;
  LSD.len = 1;

  LinAct.ext = 0;
  LinAct.len = 8;

  Clutch.ext = 0;
  Clutch.len = 1;
  Clutch.id = 0x81;    
  
  Serial1.begin(9600);
  pinMode (13, OUTPUT);
  digitalWrite(13, HIGH);
  delayMicroseconds(100000);
  digitalWrite(13, LOW);
  Can0.begin(1000000);
  Can0.attachObj(&canListener1);
  canListener1.attachGeneralHandler();

  // xTaskCreate(HealthSendFrmNav, "HSN", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(HealthSendFrmCtrl, "HSC", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(HealthSendFrmComm, "HSComm", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(SysStateFrmNav, "SSNav", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(SysStateFrmCtrl, "SSCtrl", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(SysStateFrmComm, "SSComm", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(SysStateAckFrmNav, "SSAckNav", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(SysStateAckFrmCtrl, "SSAckCtrl", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(SysStateAckFrmComm, "SSAckComm", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(BrakingOnline, "BO", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(LSDOnline, "LO", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(LinActuatorOnline, "LinActO", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(ClutchOffline, "COff", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(BrakingOffline, "BOff", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(LSDOffline, "LSDOff", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(LinActuatorOffline, "LinActOff", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(BrakingOnline, "Braking", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  vTaskStartScheduler();

  for(;;)
  return 0;
}