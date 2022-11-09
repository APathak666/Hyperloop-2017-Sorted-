// a. required for FreeRTOS
#include <kinetis.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
// b. required for Temperature
#include <Temperature.h>
#include <Wire.h>

uint16_t temp;
float tempmag;
byte address = 0x18;

void Temperaturesetup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(400000L);
}

void Temp(void* args)
{
    const TickType_t xDelay250ms = pdMS_TO_TICKS( 250 );
    for(;;) {
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
    vTaskDelay( xDelay250ms );//tConv for 0.0625
  }
}
