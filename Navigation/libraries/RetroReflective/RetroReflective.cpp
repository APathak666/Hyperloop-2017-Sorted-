// a. required for FreeRTOS
#include <kinetis.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include<Wire.h>
// b. required for RetroReflective
#include <RetroReflective.h>

// pin number
const byte interrupt_pin=4;

// all variables changing in isr must be declared volatile
volatile int strip_count=0;// no of strips passed
volatile int isr_call=0; // number of times isr is called
volatile long timer_begin=0; // timer at beginning of the strip
volatile long timer_end=0;// timer at the end of the strip

double time_width;
double velocity;
double strip_width=0.1016; // 4 inch strip

int pos[]={0,1200,2400,3600,4800,6000,7200,8400,9600,10800,12000,12008,12016,12024,12032,12040,12048,12054,12064,12072,13200,14400,15600,16800,18000,18008,18016,18024,18032,19200,20400,21600,22800};// array with position of strips corresponding to strip_count

void RetroReflectivesetup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(interrupt_pin,INPUT);
  attachInterrupt(digitalPinToInterrupt(interrupt_pin),retro_isr1,CHANGE);
}

void RetroReflective(void * args) {
  for(;;){
    // put your main code here, to run repeatedly:
    if(isr_call%2==0 && isr_call!=0){
      strip_count++;                  // count of strips crossed
      time_width=timer_end-timer_begin;// time difference for crossing a strip
      time_width=time_width/1000000;// convert micro seconds to seconds
      velocity=strip_width/time_width;// velocity in m/s
      isr_call=0;// or else this block keeps executing
    }
    else {
      timer_begin=timer_end;
    }
  }
}
void retro_isr1(){
  isr_call++; // number of times isr is called
  timer_end=micros();// time when isr is called
}
