// FlexCAN(recieve) with interrupt(masking and filter included) on teensy 3.6 (arduino code)
#include <FlexCAN.h>


class ExampleClass : public CANListener 
{
public:
   void printFrame(CAN_message_t &frame, int mailbox);
   void gotFrame(CAN_message_t &frame, int mailbox); //overrides the parent version so we can actually do something
};

void ExampleClass::printFrame(CAN_message_t &frame, int mailbox)
{
    Serial.print("ID: ");
   Serial.print(frame.id, HEX);
   Serial.print(" Data: ");
   for (int c = 0; c < frame.len; c++) 
   {
      Serial.print(frame.buf[c], HEX);
      Serial.write(' ');
   }

   Serial.write('\r');
   Serial.write('\n');
   
}

void ExampleClass::gotFrame(CAN_message_t &frame, int mailbox)
{
    
   pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delayMicroseconds(1000000);
    digitalWrite(13, LOW);
  delayMicroseconds(1000000);
   printFrame(frame, mailbox);
}

ExampleClass exampleClass;

// -------------------------------------------------------------
void setup(void)
{
 delayMicroseconds(100000);
 Serial.begin(9600);
  Serial.println(F("Another reciever"));

 pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delayMicroseconds(1000000);
    digitalWrite(13, LOW);
  delayMicroseconds(1000000);


  //if using enable pins on a transceiver they need to be set on
  pinMode(2, OUTPUT);
  pinMode(35, OUTPUT);

  digitalWrite(2, HIGH);
  digitalWrite(35, HIGH);
 Can0.begin(500000); 
  Can0.attachObj(&exampleClass);
 Can0.setNumTxBoxes(10);
  exampleClass.attachMBHandler(4);

	CAN_filter_t filter;
	filter.id=0x00001560;
	filter.flags.extended=0;
	filter.flags.remote=0;
	filter.flags.reserved=0;



  
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delayMicroseconds(1000000);
    digitalWrite(13, LOW);
  delayMicroseconds(1000000);

Can0.setFilter(filter,4);
Can0.setMask(0x1FFFFFF0,4);

pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delayMicroseconds(1000000);
    digitalWrite(13, LOW);
  delayMicroseconds(1000000);
 

}


// -------------------------------------------------------------
int main()
{
setup();
while(1){}
return 0;
}
