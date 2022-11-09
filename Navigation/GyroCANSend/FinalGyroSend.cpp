#include <kinetis.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <FlexCAN.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <i2c_t3.h>
#include <Arduino.h>
class CanListener1 : public CANListener 
{
public:
	bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

CanListener1 canListener1;
CAN_message_t GyroPkt1;


bool CanListener1::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
	digitalWrite(13, HIGH);
	delayMicroseconds(500000);
	digitalWrite(13, LOW);
	Serial1.println(frame.id);
	for (int i=0; i<8; i++)
		Serial1.println(frame.buf[i]);	
	return true;
}


Adafruit_BNO055 bno1 = Adafruit_BNO055(55,0x28);
imu::Vector<3> eul1;
int ledPin = 13;

void Orientation(void* args){

 if(!bno1.begin())
 {
   Serial1.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
 }

 for(;;)
 {
   eul1 = bno1.getVector(Adafruit_BNO055::VECTOR_EULER);
   Serial1.print("X :");
   Serial1.print(eul1[0]);
   Serial1.print("Y :");
   Serial1.print(eul1[1]);
   Serial1.print("Z :");
   Serial1.println(eul1[2]);
   *GyroPkt1.buf=eul1[0];
   //GyroPkt1.buf[1]=(uint8_t)eul1[0];
   *(GyroPkt1.buf+ 2)=eul1[1];
   //GyroPkt1.buf[3]=(uint8_t)eul1[0];
   *(GyroPkt1.buf+4)=eul1[2];
   //GyroPkt1.buf[5]=(uint8_t)eul1[0];
   //GyroPkt1.buf[6]=(uint8_t)eul1[0];
   //GyroPkt1.buf[7]=(uint8_t)eul1[0];
   //*(GyroPkt1.buf + 2)=eul1[1];
   //*(GyroPkt1.buf + 4)=eul1[2];
   Can0.write(GyroPkt1);
   //digitalWrite(ledPin, HIGH);
   //delayMicroseconds(5000000);
   //digitalWrite(ledPin, LOW);
   vTaskDelay(pdMS_TO_TICKS(1000));
 }
}

int main()
{
 // create the tasks
GyroPkt1.ext = 0;
GyroPkt1.id = 0x01;
GyroPkt1.len = 8;

Serial1.begin(9600);
pinMode(ledPin, OUTPUT);  

Can0.begin(1000000);
Can0.attachObj(&canListener1);
canListener1.attachGeneralHandler();

xTaskCreate(Orientation, "GYRO", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

vTaskStartScheduler();
for (;;);
return 0;
}


