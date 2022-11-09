#include <kinetis.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <Arduino.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
#include <event_groups.h>

#define Serial_BAUD 9600

EventGroupHandle_t grp;
int n=0;

void setup();
void task1(void * args);
void isr1();
void callee();

void task1(void * args){
  while(1){
    xEventGroupWaitBits(grp,0x01,pdTRUE,pdTRUE,portMAX_DELAY);
    Serial1.print("task n=");
    Serial1.println(n);
    //vTaskDelay(pdMS_TO_TICKS(3000));
  }

}

void isr1(){
  BaseType_t xHigherPriorityTaskWoken=pdFALSE;
  //Serial1.println("interupted");
  if(Serial4.available()>0){
    n=Serial4.read();    
    xEventGroupSetBitsFromISR(grp, 1UL << 0UL, &xHigherPriorityTaskWoken);
  }
  //Serial1.println(n);

}

void setup(){
  Serial1.begin(Serial_BAUD);
  Serial4.begin(Serial_BAUD);
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  delayMicroseconds(1000000);
  digitalWrite(13,LOW);
  xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  attachInterrupt(31,isr1,CHANGE);
  grp=xEventGroupCreate();
}

int main(){
  setup();
  Serial1.println("main started");
  vTaskStartScheduler();
  for(;;);  
  return 0;
}