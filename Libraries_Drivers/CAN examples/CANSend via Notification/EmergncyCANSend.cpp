#include <kinetis.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <Arduino.h>
#include <FlexCAN.h>

#define Serial_BAUD 9600

TaskHandle_t E=NULL, L=NULL;              //Declare Task Handles
class EmergencyAckListener : public CANListener 
{
public:
  bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

EmergencyAckListener canListener1;
CAN_message_t msg;


bool EmergencyAckListener::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
  digitalWrite(13, HIGH);
  delayMicroseconds(500000);
  digitalWrite(13, LOW);
  Serial1.println(frame.id);
  for (int i=0; i<8; i++)
    Serial1.println((char)frame.buf[i]);

  return true;
}

void Emergency(void * args);
void Logging(void * args);


void Logging(void* args)
{
   while(1)
   { 
      for (int i=0;i<100;i++)
      {
        if (i%7==0)
        {
            Serial1.println("Emergency!!");
            xTaskNotifyGive(E);
            vTaskDelay(pdMS_TO_TICKS(3000));
        }
        else
          Serial1.println("Logging...");
      }
   }
}

void Emergency(void* args)
{   
  while(1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    Serial1.println("Emergency handled");
    Can0.write(msg);
    //digitalWrite(13,HIGH);
    //delayMicroseconds(1000000);
    //digitalWrite(13,LOW);
    //vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

int main()
{
  msg.ext = 0;
  msg.id = 0x01;
  msg.len = 8;
  
  msg.buf[0] = 'E';
  msg.buf[1] = 'm';
  msg.buf[2] = 'r';
  msg.buf[3] = 'g';
  msg.buf[4] = 'n';
  msg.buf[5] = 'c';
  msg.buf[6] = 'y';
  msg.buf[7] = 0;  
  Serial1.begin(Serial_BAUD);
  Serial1.println("main started");
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  delayMicroseconds(1000000);
  digitalWrite(13,LOW);
  
  Can0.begin(1000000);
  Can0.attachObj(&canListener1);
  canListener1.attachGeneralHandler();
  
  xTaskCreate(Emergency, "Emergency", configMINIMAL_STACK_SIZE, NULL, 4, &E);
  xTaskCreate(Logging, "Logging", configMINIMAL_STACK_SIZE, NULL, 1, &L);
  vTaskStartScheduler();
  
  for(;;);  
  return 0;
}