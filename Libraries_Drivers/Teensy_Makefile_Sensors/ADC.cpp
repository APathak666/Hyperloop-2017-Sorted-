#include <kinetis.h>
#include <FreeRTOS.h>
#include <task.h>
#include <Arduino.h>
#include <SPI.h>

#define CH0 0x08
#define CH1 0x09
#define CH2 0x0A
#define CH3 0x0B
#define CH4 0x0C
#define CH5 0x0D
#define CH6 0x0E
#define CH7 0x0F

#define CS 10
#define MISO 12
#define MOSI 11
#define CLK 13

double vRef        = 5;
int    adc_reading = 0;
byte dataMSB =0;
byte dataLSB =0;
byte JUNK = 0;
double voltage = 0;
int8_t readAddress;

int adc_mcp_3008()
{
	SPISettings adc(3600000,MSBFIRST,SPI_MODE0);
	readAddress = CH0;
	SPI.begin();
	pinMode(CS,OUTPUT);
	digitalWrite(CS,LOW);
	digitalWrite(CS,HIGH);
	SPI.beginTransaction(adc);
	digitalWrite(CS,LOW);
	Serial.begin(9600);


	for (;;)
	{
		  SPI.beginTransaction (adc);
		  digitalWrite         (CS, LOW);
		  SPI.transfer         (0x01);                                 // Start Bit
		  dataMSB =            SPI.transfer(readAddress << 4) & 0x03;  // Send readAddress and receive MSB data, masked to two bits
		  dataLSB =            SPI.transfer(JUNK);                     // Push junk data and get LSB byte return
		  digitalWrite         (CS, HIGH);
		  SPI.endTransaction   ();
		 
		  adc_reading = dataMSB << 8 | dataLSB;
		  voltage = adc_reading*vRef/1024;

		  Serial.println(adc_reading);
		  delayMicroseconds(250000);

			/* code */
	}
	return 0;
}
