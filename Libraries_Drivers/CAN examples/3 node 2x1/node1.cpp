#include <FlexCAN.h>

volatile int lol = 0;
volatile int kek = 0;
volatile int gg = 0;
volatile int got2 = 0;
volatile int got3 = 0;
CAN_message_t msg;

class CanListener1 : public CANListener 
{
public:
	bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

CanListener1 canListener1;

bool CanListener1::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
	//cli();
	gg=!gg;
	//digitalWrite(36, gg);
	if(frame.buf[7]=='0')
	{
		lol=!lol;
		digitalWrite(13, lol);
		got2=1;
	}

	else if(frame.buf[7]=='F')
	{
		kek=!kek;
		digitalWrite(37, kek);
		got3=1;
	}

	if(got2&got3)
	{
		msg.buf[0] = frame.buf[0]+1;
		Can0.write(msg);
		got3=0;
		got2=0;
	}
	//sei();
	gg=!gg;
	//digitalWrite(36, gg);
	return true;
}

int main()
{
	msg.ext = 0;
	msg.id = 0x01;
	msg.len = 8;
	msg.buf[0] = 0;
	msg.buf[1] = 'n';
	msg.buf[2] = 'o';
	msg.buf[3] = 'd';
	msg.buf[4] = 'e';
	msg.buf[5] = ':';
	msg.buf[6] = ' ';
	msg.buf[7] = '1';

	pinMode(13, OUTPUT);
	pinMode(36, OUTPUT);
	pinMode(37, OUTPUT);
	Can0.begin(1000000);
	Can0.attachObj(&canListener1);
	canListener1.attachGeneralHandler();

	Can0.write(msg);

	CAN_filter_t filter;
	//Framehandler invoked only for id 560
	filter.id=0x20;
	filter.flags.extended=0;
	filter.flags.remote=0;
	filter.flags.reserved=0;
	Can0.setFilter(filter,0);
	Can0.setMask(0x20,0);
	Can0.write(msg);
	while(1){}
	return 0;
}