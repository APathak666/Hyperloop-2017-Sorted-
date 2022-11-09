#include <i2c_t3.h>
#include <Arduino.h>
//#include <Wire.h>


uint8_t address1 =0x28;
size_t len1 = 2;
float pressure;
uint32_t timeout_P =200;

#define Serial Serial1

void honey_well()
{

	Serial.begin(9600);
	Serial.println("Hello");
	Wire.begin();
	Wire.setClock(400000L);
	Serial.println("Beginning");

	for(;;)
	{	uint16_t out;
		//Serial.println("lol1");
		Wire.beginTransmission(address1);
		Wire.endTransmission(timeout_P);
		//Serial1.println("lolwa");
		Wire.requestFrom(address1,len1);
		//Serial1.println("lol2");
		//Serial1.println(out,HEX);
		while(Wire.available())
		{	
			out= Wire.read();
			//Serial1.println(out,HEX);
			out = out<<8;
			//Serial1.println(out,HEX);
			out = out|Wire.read();
			//Serial1.println(out,HEX);
			//Serial.println("lol3");
		}

		Serial.println(out,HEX);
		pressure =(out - 0x0666)*1.6/(0x3999 - 0x0666);
		Serial.println(pressure);
		delayMicroseconds(250000);
		//Serial1.println("End");
	}

}
