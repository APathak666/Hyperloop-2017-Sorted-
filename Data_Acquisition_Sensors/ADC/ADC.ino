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

SPISettings adc(3600000,MSBFIRST,SPI_MODE0);

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
SPI.begin();
pinMode(CS,OUTPUT);
digitalWrite(CS,LOW);
digitalWrite(CS,HIGH);
SPI.beginTransaction(adc);
digitalWrite(CS,LOW);
}

void loop()
{
  
  double vRef        = 5;
  int    adc_reading = 0;
 
  adc_reading        = adc_single_channel_read (CH0);
 
  Serial.print       ("ADC Ch ");
  Serial.print       (CH0 & 0x07);
  Serial.print       (" Voltage: ");
  Serial.println     ((adc_reading * vRef) / 1024, 4);
 
  delay(50);
  
}
 
int adc_single_channel_read(byte readAddress)
{
 
  byte dataMSB =    0;
  byte dataLSB =    0;
  byte JUNK    = 0x00;
  
  SPI.beginTransaction (adc);
  digitalWrite         (CS, LOW);
  SPI.transfer         (0x01);                                 // Start Bit
  dataMSB =            SPI.transfer(readAddress << 4) & 0x03;  // Send readAddress and receive MSB data, masked to two bits
  dataLSB =            SPI.transfer(JUNK);                     // Push junk data and get LSB byte return
  digitalWrite         (CS, HIGH);
  SPI.endTransaction   ();
 
  return               dataMSB << 8 | dataLSB;
 
}

