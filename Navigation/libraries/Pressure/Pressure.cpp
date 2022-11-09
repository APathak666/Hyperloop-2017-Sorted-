#include <kinetis.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
// b. required for IMU
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Pressure.h>

uint16_t output;
byte address_pressure=0x28;
float pressure;
void Pressuresetup(void) {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire1.begin();
  Wire1.setClock(400000L);
}


void Pressure(void* args)
{
  for(;;)
  {
    // put your main code here, to run repeatedly:
    Wire1.requestFrom(address_pressure,(byte)2);
    while(Wire1.available())
    {
      output=Wire1.read();
      output=output<<8;
      output=output|Wire1.read();
    }
    output=output&0x0fff;
    pressure=((output/0x3fff)-0.1)/0.8;
    Serial.println(pressure);
  }
}
