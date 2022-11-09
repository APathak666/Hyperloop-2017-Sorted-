#include <Arduino.h>
#include <FlexCAN.h>

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
		Serial1.println((char)frame.buf[i]);
	Can0.write(msg);	
	return true;
}

int main()
{
	msg.ext = 0;
	msg.id = 0x01;
	msg.len = 8;

	msg.buf[0] = 'M';
	msg.buf[1] = 's';
	msg.buf[2] = 'g';
	msg.buf[3] = 'R';
	msg.buf[4] = 'c';
	msg.buf[5] = 'v';
	msg.buf[6] = 'd';
	msg.buf[7] = 0;

	Serial1.begin(9600);
	pinMode (13, OUTPUT);
	digitalWrite(13, HIGH);
	delayMicroseconds(500000);
	digitalWrite(13, LOW);
	
	Can0.begin(1000000);
	Can0.attachObj(&canListener1);
	canListener1.attachGeneralHandler();
	
	return 0;
}