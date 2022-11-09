

// pin number
const byte interrupt_pin_1 = 4;
const byte interrupt_pin_2 = 5; 

// all variables changing in isr must be declared volatile
//volatile int strip_count=0;// no of strips passed
volatile int isr_call_1=0; // number of times retro_isr1 is called
volatile int isr_call_2=0; //number of times retro_isr2 is called
volatile long time_start=0; // timer at beginning of the strip
volatile long time_end=0;// timer at the end of the strip

double time_width;
double velocity;
//double strip_width=0.1016; // 4 inch strip
double pod_length=3;//distance between the 2 retro reflective sensors in meters
int pod_position;// position within the tube

//int pos[]={0,1200,2400,3600,4800,6000,7200,8400,9600,10800,12000,12008,12016,12024,12032,12040,12048,12054,12064,12072,13200,14400,15600,16800,18000,18008,18016,18024,18032,19200,20400,21600,22800};// array with position of strips corresponding to strip_count

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(interrupt_pin_1,INPUT);
pinMode(interrupt_pin_1,INPUT);
attachInterrupt(digitalPinToInterrupt(interrupt_pin_1),retro_isr1,RISING);
attachInterrupt(digitalPinToInterrupt(interrupt_pin_2),retro_isr2,RISING);
}

void loop() {
  // put your main code here, to run repeatedly:

  
/*if(isr_call%2==0 && isr_call!=0){
  strip_count++;                  // count of strips crossed
  time_width=timer_end-timer_begin;// time difference for crossing a strip
  time_width=time_width/1000000;// convert micro seconds to seconds
  velocity=strip_width/time_width;// velocity in m/s
  isr_call=0;// or else this block keeps executing
}
else{
timer_begin=timer_end;
}*/
if (isr_call_1 !=0 && isr_call_2!=0 && isr_call_1 == isr_call_2)
{
  time_width = (time_end-time_start)/1000000;// time between the 2 isr calls
  velocity = pod_length/time_width;
  pod_position = isr_call_1*100; // position of the rear retro_reflective sensor in feet
}
}

void retro_isr1(){
//cli(); ? to disable interrupt, while isr is executing
isr_call_1++; // number of times isr is called
time_start=micros();// time when isr is called
//sei() ? to enable interrupt
}

void retro_isr2(){
  //cli();
  isr_call_2++;
  time_end = micros();
  //sei();
}

