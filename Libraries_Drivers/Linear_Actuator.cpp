#include <Arduino.h>
#define dir 3
#define pwm 4
void dc_motor_test()
{
pinMode(dir,OUTPUT);
pinMode(pwm,OUTPUT);

digitalWrite(dir,LOW);

digitalWrite(pwm,LOW);
delayMicroseconds(2000000);

for(;;)
{
analogWrite(pwm,254);
delayMicroseconds(1000);
}
}
