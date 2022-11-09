#include <kinetis.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <Arduino.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>

#define Serial_BAUD 9600

SemaphoreHandle_t accMutex;

void setup();
void task1(void * args);
void task2(void * args);

void task1(void * args){
  while(1){
    xSemaphoreTake( accMutex, portMAX_DELAY );
    Serial1.println("task1 in");
    delayMicroseconds(1000000);
    Serial1.println("task1 out");
    xSemaphoreGive( accMutex );
    vTaskDelay(pdMS_TO_TICKS(3000));
  }

}

void task2(void * args){
  while(1){
    xSemaphoreTake( accMutex, portMAX_DELAY );
    Serial1.println("task2 in");
    delayMicroseconds(1000000);
    Serial1.println("task2 out");
    xSemaphoreGive( accMutex );
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

void setup(){
  Serial1.begin(Serial_BAUD);
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  delayMicroseconds(1000000);
  digitalWrite(13,LOW);
  xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(task2, "task2", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  accMutex = xSemaphoreCreateMutex();
}

int main(){
  setup();
  Serial1.println("main started");
  vTaskStartScheduler();
  for(;;);  
  return 0;
}