// FlexCAN(recieve) with interrupt on teensy 3.6 (arduino code)
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
    printFrame(frame, mailbox);
   pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delayMicroseconds(100000);
    digitalWrite(13, LOW);
  delayMicroseconds(100000);
   
}

ExampleClass exampleClass;

// -------------------------------------------------------------
void setup(void)
{
 delayMicroseconds(5000000);
 Serial.begin(9600);
  Serial.println(F("Hello Teensy 3.6 dual CAN Test With Objects."));

  Can0.begin(500000);  


  //if using enable pins on a transceiver they need to be set on
  pinMode(2, OUTPUT);
  pinMode(35, OUTPUT);

  digitalWrite(2, HIGH);
  digitalWrite(35, HIGH);

  Can0.attachObj(&exampleClass);
  exampleClass.attachGeneralHandler();

  
}


// -------------------------------------------------------------
void loop(void)
{
;
}
