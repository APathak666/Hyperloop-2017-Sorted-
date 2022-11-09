// FlexCAN(requester) with interrupt on teensy 3.2 to recieve a message with specific id (arduino code)
#include <FlexCAN.h>
static CAN_message_t msg;

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
    printFrame(frame, mailbox);
   pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delayMicroseconds(1000000);
    digitalWrite(13, LOW);
  delayMicroseconds(1000000);
   
}

ExampleClass exampleClass;

// -------------------------------------------------------------
void setup(void)
{
 delayMicroseconds(5000000);
 Serial.begin(9600);
  Serial.println(F("Another reciever"));

  Can0.begin(500000);  


  //if using enable pins on a transceiver they need to be set on
  pinMode(2, OUTPUT);
  pinMode(35, OUTPUT);

  digitalWrite(2, HIGH);
  digitalWrite(35, HIGH);

  Can0.attachObj(&exampleClass);
  exampleClass.attachGeneralHandler();
 
  msg.ext = 0;
  msg.id = 0x100;
  msg.len = 8;
  msg.buf[0] = 100;
  msg.buf[1] = 20;
  msg.buf[2] = 0;
  msg.buf[3] = 100;
  msg.buf[4] = 128;
  msg.buf[5] = 64;
  msg.buf[6] = 32;
  msg.buf[7] = 16;
  Can0.write(msg); 
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delayMicroseconds(1000000);
    digitalWrite(13, LOW);
  delayMicroseconds(1000000);
}


// -------------------------------------------------------------
void loop(void)
{
;
}
