/* The only difference between this commit and the previous commit of this file is that here I have taken the offset into consideration. At 0 current
flowing, the ADC was supposed to give me a reading of 512. However when tested it gave a reading ranging between 540 and 544. Thus I shifted it by 31 units and assumed that shifted value to be the 0 current. Apart from that I am averaging over 100 values. 

Connections with teensy 

ADC 	Teensy
10		10
11		11
12		12
13		13

Give a supply of 5V into Vref of ADC and conect AGND and DGND to the common ground. Give 5V Vcc to the hall sensor. Connect output of hall sensor into CH0 of ADC.

*/



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
double current;
double final_current;
double final_adc;

int main()
{
	SPISettings adc(3600000,MSBFIRST,SPI_MODE0);
	readAddress = CH0;
	SPI.begin();
	pinMode(CS,OUTPUT);
	digitalWrite(CS,LOW);
	digitalWrite(CS,HIGH);
	SPI.beginTransaction(adc);
	digitalWrite(CS,LOW);
	Serial1.begin(9600);

	int i=0;

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
//		  voltage = adc_reading*vRef/1024;
		  current = (adc_reading-30-512)*75.8;
		  final_current+=current;
		  final_adc+=adc_reading;
		  if(i%99==0){
		  	Serial1.print(final_adc/100);
		  	Serial1.print(": ");
		  	Serial1.println(final_current/100);
		  	final_current=0;
		  	final_adc=0;
		  	i=0;
		  	delayMicroseconds(100000);
		  }
		  i++;
			/* code */
	}
	return 0;
}