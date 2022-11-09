#include <Arduino.h>
#define pul 6

void stepper_motor_test()
{
pinMode(pul,OUTPUT);
pinMode(7,OUTPUT);
pinMode(8,OUTPUT);
analogWriteResolution(15);
analogWriteFrequency(pul,1500);
analogWrite(pul,16384);
digitalWrite(7,HIGH);
digitalWrite(8,HIGH);
for(;;)
{
/*digitalWrite(pul,HIGH);
delayMicroseconds(500);
digitalWrite(pul,LOW);
delayMicroseconds(500);*/
}
}
