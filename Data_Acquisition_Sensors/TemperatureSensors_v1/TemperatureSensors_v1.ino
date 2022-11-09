#include <Wire.h>

uint16_t temp;
float tempmag;
byte address = 0x18;
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
Wire.begin();
Wire.setClock(400000L);
}

void loop() {
  // put your main code here, to run repeatedly:
Wire.beginTransmission(address|0x00);
Wire.write(0x05);
Wire.endTransmission();
Wire.requestFrom(address|0x00,2);// if you put Wire.beginTra... then you'll have to start again
while(Wire.available())
{
  temp=Wire.read();
  temp = temp<<8;
  temp=temp|Wire.read();
}

tempmag=temp&0x0fff;
tempmag /=16;

if(temp&0x1000)
{
  tempmag=256-tempmag;
}

Serial.println(tempmag);

delay(250);//tConv for 0.0625
}
