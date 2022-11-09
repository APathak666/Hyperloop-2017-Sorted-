#include <FreeRTOS.h>
#include <task.h>
#include <Arduino.h>
//#include <usb_dev.h>
#include <kinetis.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <Adafruit_BNO055.h>


Adafruit_BNO055 bno1 = Adafruit_BNO055(55,0x28);
imu::Vector<3> eul1;
int ledPin =13;

void bosch_bno055_main(void *args)
{
	Serial.begin(9600);
	if(!bno1.begin())
	{
		Serial.println("No bno detected");
	}
	Serial.println("Welcome");

	for(;;)
	{
		eul1 = bno1.getVector(Adafruit_BNO055::VECTOR_EULER);
		Serial.print("values\t");
		Serial.println("eul[1]");
		vTaskDelay(500/portTICK_PERIOD_MS);
	}

}
