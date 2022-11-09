#include <kinetis.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <FreeRTOS.h>
#include <task.h>

#define Serial Serial1
#define BNO055_SAMPLERATE_DELAY_MS (100)	// Delay between data requests

volatile double xx_,yy_,zz_,ww_;

Adafruit_BNO055 bno = Adafruit_BNO055();	// Create sensor object bno based on Adafruit_BNO055 library

void SensorTask(void* args) {
	Serial.begin(115200);					// Begin serial port communication
	if(!bno.begin())						// Initialize sensor communication
	{
		Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
	}
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	bno.setExtCrystalUse(true);				// Use the crystal on the development board

	for(;;) {
		imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
		xx_=euler.x();
		yy_=euler.y();
		zz_=euler.z();
		//ww_=euler.w();
		vTaskDelay(BNO055_SAMPLERATE_DELAY_MS);	// Pause before capturing new data
	}
}