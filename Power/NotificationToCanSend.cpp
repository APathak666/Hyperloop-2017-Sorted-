#include <kinetis.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <Arduino.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>

#define Serial_BAUD 9600

TaskHandle_t E=NULL, L=NULL;              //Declare Task Handles

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
  }
}

int main()
{  
  Serial1.begin(Serial_BAUD);
  Serial1.println("main started");
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  delayMicroseconds(1000000);
  digitalWrite(13,LOW);
  
  xTaskCreate(Emergency, "Emergency", configMINIMAL_STACK_SIZE, NULL, 4, &E);
  xTaskCreate(Logging, "Logging", configMINIMAL_STACK_SIZE, NULL, 1, &L);
  vTaskStartScheduler();
  
  for(;;);  
  return 0;
}