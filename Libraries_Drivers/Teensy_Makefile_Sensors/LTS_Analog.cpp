#include <stdlib.h>
#include <kinetis.h>
#include <FreeRTOS.h>
#include <task.h>
#include <Arduino.h>

#define ON 30
#define HIGH_V 3.2	
#define LOW_V 0.64
#define HIGH_D 8.5
#define LOW_D 3.5
#define Serial Serial1
//byte state = 1;

float distance;
float v = 0;


void optoNCDT_1320_Analog()
{
	pinMode(ON,OUTPUT);
	pinMode(A18,INPUT);
	pinMode(13,OUTPUT);
	analogReadResolution(16);
	digitalWrite(ON,HIGH);
	Serial1.begin(9600);
	
	for(;;)
	{	analogReadResolution(16);
		v = analogRead(A18)*3.3/65535;
		//digitalWrite(13,state);
		/*if(v > 0.55)
			digitalWrite(13,HIGH);
		else
			digitalWrite(13,LOW);*/
		Serial1.print(v);
		Serial1.print("\t");
		distance = (v-LOW_V)/(HIGH_V - LOW_V) *(HIGH_D - LOW_D) + LOW_D;
		Serial1.println(distance,4);
		//state !=state;
		delayMicroseconds(1000000);
	}
}

