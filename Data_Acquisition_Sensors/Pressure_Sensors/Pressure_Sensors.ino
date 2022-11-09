#include<Wire.h>

uint16_t output;
byte address=0x28;
float pressure;
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
Wire.begin();
Wire.setClock(400000L);
}

void loop() {
  // put your main code here, to run repeatedly:
Wire.requestFrom(address,2);
while(Wire.available())
{
  output=Wire.read();
  output=output<<8;
  output=output|Wire.read();
}
Serial.print(output);
Serial.print("\t");
pressure=output/float(13575);
//output=output&0x0fff;
//pressure=((output/0x3fff)-0.1)/0.8;
Serial.print(pressure,6);
Serial.print("  atm");
if(minp>output)
  minp = output;
Serial.println(minp/float(13575));


}
