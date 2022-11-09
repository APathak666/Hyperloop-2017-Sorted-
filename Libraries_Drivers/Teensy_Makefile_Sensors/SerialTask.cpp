#include <FreeRTOS.h>
#include <task.h>

#include <Arduino.h>
#include <usb_dev.h>

#include <utility/imumaths.h>
#define Serial Serial1

extern volatile double xx_,yy_,zz_,ww_;
volatile double *x=&xx_,*y=&yy_,*z=&zz_,*w=&ww_;

void SerialTask(void* args) {
  // initialize the serial
  usb_init();
  Serial1.begin(115200);

  for (;;) {
    Serial.print(*w, 4);	Serial.print("\t");// Print quaternion w
	Serial.print(*x, 4);	Serial.print("\t");// Print quaternion x
	Serial.print(*y, 4);	Serial.print("\t");// Print quaternion y
	Serial.print(*z, 4);	Serial.println();// Print quaternion z
	Serial.flush();
    vTaskDelay(1000);
  }
}