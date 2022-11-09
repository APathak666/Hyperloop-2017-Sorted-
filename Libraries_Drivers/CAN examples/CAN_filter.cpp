//CAN Send and receive using filter

//------------------------------------------------------------------------------------------------------------------------------------------
// Teensy 1

#include <Arduino.h>
#include <FlexCAN.h>


CAN_filter_t filter;

class CanListener1 : public CANListener 
{
public:
	bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

CanListener1 canListener1;
CAN_message_t msg;


bool CanListener1::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
	digitalWrite(13, HIGH);
	delayMicroseconds(500000);
	digitalWrite(13, LOW);
	Serial1.println(frame.id);
	for (int i=0; i<8; i++)
		Serial1.print((char)frame.buf[i]);
	Serial1.println();	
	return true;
}

int main(){
	filter.id=0x01;
	filter.flags.extended=0;
	filter.flags.remote=0;
	filter.flags.reserved=0;

	msg.ext = 0;
	msg.id = 0x01;
	msg.len = 8;
	msg.buf[0] = 0;
	msg.buf[1] = 'C';
	msg.buf[2] = 'h';
	msg.buf[3] = 'e';
	msg.buf[4] = 'c';
	msg.buf[5] = 'k';
	msg.buf[6] = ':';
	msg.buf[7] = '2';

	Serial1.begin(9600);
	pinMode (13, OUTPUT);
	digitalWrite(13, HIGH);
	delayMicroseconds(500000);
	digitalWrite(13, LOW);
	Can0.begin(1000000);
	Can0.attachObj(&canListener1);
	canListener1.attachMBHandler(0);
	Can0.setFilter(filter,0);
	Can0.setMask(0x01,0);
//	canListener1.attachGeneralHandler();
	while(1)
	{
		Can0.write(msg);
		delayMicroseconds(1000000);
	}
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

//Teensy 2

#include <Arduino.h>
#include <FlexCAN.h>

class CanListener1 : public CANListener 
{
public:
	bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

CanListener1 canListener1;
CAN_message_t msg;
CAN_message_t msg1;


bool CanListener1::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
	digitalWrite(13, HIGH);
	delayMicroseconds(500000);
	digitalWrite(13, LOW);
	Serial1.println(frame.id);
	for (int i=0; i<8; i++)
		Serial1.println((char)frame.buf[i]);	
	return true;
}

int main(){
	Serial1.begin(9600);
	pinMode (13, OUTPUT);
	Can0.begin(1000000);

	digitalWrite(13, HIGH);
	delayMicroseconds(500000);
	digitalWrite(13, LOW);

	msg.ext = 0;
	msg.id = 0x03;
	msg.len = 8;
	msg.buf[0] = 0;
	msg.buf[1] = 'C';
	msg.buf[2] = 'h';
	msg.buf[3] = 'e';
	msg.buf[4] = 'c';
	msg.buf[5] = 'k';
	msg.buf[6] = ':';
	msg.buf[7] = '3';

	msg1.ext = 0;
	msg1.id = 0x02;
	msg1.len = 8;
	msg1.buf[0] = 0;
	msg1.buf[1] = 'C';
	msg1.buf[2] = 'h';
	msg1.buf[3] = 'e';
	msg1.buf[4] = 'c';
	msg1.buf[5] = 'k';
	msg1.buf[6] = ':';
	msg1.buf[7] = '2';

	Can0.attachObj(&canListener1);
	canListener1.attachGeneralHandler();
	while(1)
	{
		Can0.write(msg);
		Can0.write(msg1);
		delayMicroseconds(1000000);
	}
	return 0;
}

