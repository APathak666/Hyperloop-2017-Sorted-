#include <kinetis.h>
#include <stdlib.h>
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <FlexCAN.h>
#include <queue.h>
#include <timers.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
Timer1 : 100 feet/V + __

Timer2 : 3 feet/V + ___

Timer3 : 
*/
TaskHandle_t CANS=NULL, BPID=NULL;

void Emergency (void* args)
{
  ulTaskNotifyWait()
}

void isrService1(){
  if(flag==0){
    //xTaskNotifyGive(TASK1);
    Task1();
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
    for(;;){
      //ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
      rrTimer1.end();
      rr_flag=1;
      xSemaphoreTake( accMutex, portMAX_DELAY );
      local_velocity = global_velocity;
      local_position = global_position;
      local_stripe_count = stripe_count;
      xSemaphoreGive( accMutex );
      rrTimer2.begin(Task7, 5); //DIS_BW_2_SENSORS/local_velocity
    } 
}


void Task2(){
  double local_velocity;
    uint32_t local_position;
    uint8_t local_stripe_count;
    for(;;){
      ulTaskNotifyTake (pdTRUE, portMAX_DELAY);
      rrTimer2.end();
      rr_flag=0;
      xSemaphoreTake( accMutex, portMAX_DELAY );
      local_velocity = global_velocity;
      local_position = global_position;
      local_stripe_count = stripe_count;
    xSemaphoreGive( accMutex );
    //Update pos, vel, stripe_count
    if (stripe_count == 16){
      system_state=5;
        sys_state_change.buf[0]=5;
        Can0.write(sys_state_change);
        xTaskNotifyGive(BPID);
    }
    rrTimer1.begin(Task3, 5); //DIS_BW_2_STRIPES/local_velocity
    }
}


void Task3(){
  //Read current x and stripe_count from global variables
    uint32_t local_velocity;
    uint32_t local_position;
    uint8_t local_stripe_count;
    
    for(;;){
      ulTaskNotifyTake (pdTRUE, portMAX_DELAY);
      xSemaphoreTake( accMutex, portMAX_DELAY );
      local_velocity = global_velocity;
      local_position = global_position;
      local_stripe_count = stripe_count;
      xSemaphoreGive( accMutex );
      if (local_position - local_stripe_count > 150){
        xTaskNotifyGive(E);
      }
      else{
        rr_flag=3;
        rrTimer3.begin(Task3, 5000);
        timer3_error_counter+=1;
      }
    }
}


void Task4(){
    uint32_t local_velocity;
    uint32_t local_position;
    uint8_t local_stripe_count;

    for(;;){
      ulTaskNotifyTake (pdTRUE, portMAX_DELAY);
      xSemaphoreTake( accMutex, portMAX_DELAY );
      local_velocity = global_velocity;
      local_position = global_position;
      local_stripe_count = stripe_count;
      xSemaphoreGive( accMutex );
      if (local_position - local_stripe_count > 150){
        xTaskNotifyGive(E);
      }
      else {
        rrTimer3.end();
        rr_flag = 1;
        rrTimer2.begin(Task7, 5); //DIS_BW_2_SENSORS/local_velocity
      }
    }
}


void Task5(){
  uint32_t local_position;
  uint32_t local_velocity;
  uint8_t local_stripe_count;
  for(;;){
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    xSemaphoreTake(accMutex, portMAX_DELAY);
    local_velocity=global_velocity;
    local_position = global_position;
    local_stripe_count = stripe_count;
    if(local_position - local_stripe_count > 150){
      xTaskNotifyGive(E);
    }
    else{
      //Notify the base station
      rrTimer3.end();
      sensor1_error_counter+=1;
      if (sensor1_error_counter == 2) 
          xTaskNotifyGive(E);
        rrTimer1.begin(Task3, 5); //DIS_BW_2_STRIPES/local_velocity
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



void Task6(){
  //Notify Base Station of the Error: Sensor 1 missed a stripe
  double local_velocity;
    uint32_t local_position;
    uint8_t local_stripe_count;
  for(;;){
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
      rrTimer1.end();
      sensor1_error_counter+=1;
      if (sensor1_error_counter == 2){
        xTaskNotifyGive(E);
      } 
      rrTimer1.begin(Task3,5);//DIS_BW_2_STRIPES/local_velocity
      if (stripe_count == 16){
        system_state=5;
        sys_state_change.buf[0]=5;
        Can0.write(sys_state_change);
        xTaskNotifyGive(BPID);
      }
    //Update pos, stripe_count
  }
}


void Task7(){
  double local_velocity;
    uint32_t local_position;
    uint8_t local_stripe_count;

  for(;;){
    //  Notify Base Station
      rr_flag=0;
    //  Update pos, stripe count
      xSemaphoreTake( accMutex, portMAX_DELAY );
      local_velocity = global_velocity;
      local_position = global_position;
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
      rrTimer1.begin(Task3, 5);//DIS_BW_2_STRIPES/local_velocity 
  }
}

void toggle (void* args)
{
  GPIOD_PTOR = 1<<2;
  vTaskDelay(pdMS_TO_TICKS(5000));
}

int main()
{
  Serial1.begin(9600);
  xTaskCreate(toggle, "toggle", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  vTaskSchedulerStart();
  for(;;);
  return 0;
}