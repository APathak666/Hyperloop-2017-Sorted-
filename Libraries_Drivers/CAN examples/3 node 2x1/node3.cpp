#include <FlexCAN.h>

volatile int lol = 0;
CAN_message_t msg;

class CanListener1 : public CANListener 
{
public:
	bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

CanListener1 canListener1;

bool CanListener1::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
	lol=!lol;
	digitalWrite(13, lol);
	Can0.write(msg);
	msg.buf[0] = frame.buf[0]+1;
	return true;
}

int main()
{
	msg.ext = 0;
	msg.id = 0x03;
	msg.len = 8;
	msg.buf[0] = 0;
	msg.buf[1] = 'n';
	msg.buf[2] = 'o';
	msg.buf[3] = 'd';
	msg.buf[4] = 'e';
	msg.buf[5] = ':';
	msg.buf[6] = ' ';
	msg.buf[7] = '3';

	pinMode(13, OUTPUT);
	Can0.begin(1000000);
	Can0.attachObj(&canListener1);
	canListener1.attachMBHandler(0);

	CAN_filter_t filter;
	// Framehandler invoked only for id 560
	filter.id=0x01;
	filter.flags.extended=0;
	filter.flags.remote=0;
	filter.flags.reserved=0;
	Can0.setFilter(filter,0);
	Can0.setMask(0x1FFFFFFF,0);
	//Can0.write(msg);
	while(1){}
	return 0;
}