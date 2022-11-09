#include <i2c_t3.h>
#include <Arduino.h>
//#include <Wire.h>


byte address1 =0x28;
size_t len1 = 2;
float pressure;

#define Serial Serial1

void honey_well()
{

	Serial.begin(9600);
	//Serial1.println("Hello");
	Wire.begin();
	Wire.setClock(400000L);
	//Serial1.println("Beginning");

	for(;;)
	{	uint16_t out = 0x2345;
		//Serial.println("lol1");
		Wire.beginTransmission(address1);
		Wire.endTransmission();
		Wire.requestFrom(address1,len1);
		//Serial.println("lol2");
		//Serial1.println(out,HEX);
		//while(Wire.available())
		//{	
			out= Wire.read();
			//Serial1.println(out,HEX);
			out = out<<8;
			//Serial1.println(out,HEX);
			out = out|Wire.read();
			//Serial1.println(out,HEX);
			//Serial.println("lol3");
		//}

		Serial.println(out & 0x3fff,HEX);
		delayMicroseconds(250000);
		//Serial1.println("End");
	}

}
