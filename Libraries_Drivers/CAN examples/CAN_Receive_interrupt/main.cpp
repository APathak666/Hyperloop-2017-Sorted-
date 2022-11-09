//FlexCAN with FreeRTOS working for teensy 3.6 (uses makefile)

#include <FlexCAN.h>
#include <Arduino.h>
#include <usb_dev.h>
#include<FreeRTOS.h>
#include<task.h>
//#include <Serial1.h>

class ExampleClass : public CANListener 
{
	public:
		void printFrame(CAN_message_t &frame, int mailbox);
		bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

void ExampleClass::printFrame(CAN_message_t &frame, int mailbox)
{
	Serial1.print("ID: ");
	Serial1.print(frame.id, HEX);
	Serial1.print(" Data: ");
	for (int c = 0; c <= frame.len-1; c++) 
	{
		Serial1.print(frame.buf[c], HEX);
		Serial1.write(' ');
	}
	Serial1.write('\r');
	Serial1.write('\n');
	//Serial1.flush();
}
bool lol=1;
bool ExampleClass::frameHandler(CAN_message_t &frame, int mailbox,uint8_t controller)
{
	
	printFrame(frame, mailbox);
	return true;
}
ExampleClass exampleClass;
// -------------------------------------------------------------
void setup(void)
{
	
	Serial1.begin(9600);
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);
	delayMicroseconds(1000000);
	digitalWrite(13, LOW);
	delayMicroseconds(1000000);
	digitalWrite(13, HIGH);
	delayMicroseconds(1000000);
	digitalWrite(13, LOW);
	Serial1.println(F("Hello Teensy 3.6 dual CAN Test With Objects."));
	Serial1.flush();
	delayMicroseconds(1000000);
	Can0.begin(500000);  
	Can0.attachObj(&exampleClass);
	exampleClass.attachGeneralHandler();
	
}
// -------------------------------------------------------------

static void MyTask1(void* pvParameters)
{
  while(1)
  {
    Serial1.println(F("Task1"));
	digitalWrite(13, LOW);
	delayMicroseconds(1000000);
	digitalWrite(13, HIGH);
	delayMicroseconds(1000000);
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
int main()
{
	setup();
	xTaskCreate(MyTask1, "Task1", 100, NULL, 1, NULL);
	vTaskStartScheduler();
	int i=0;
	while(1)
	{
		;
	}
	i--;
	return 0;
}

