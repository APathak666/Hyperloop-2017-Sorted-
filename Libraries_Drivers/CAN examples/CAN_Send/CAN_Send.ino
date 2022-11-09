/*
 * Object Oriented CAN example for Teensy 3.6 with Dual CAN buses 
 * By Collin Kidder. Based upon the work of Pawelsky and Teachop
 * 
 * Both buses are set to 500k to show things with a faster bus.
 * The reception of frames in this example is done via callbacks
 * to an object rather than polling. Frames are delivered as they come in.
 */

#include <FlexCAN.h>
/*
#ifndef __MK66FX1M0__
  #error "Teensy 3.6 with dual CAN bus is required to run this example"
#endif
*/
static CAN_message_t msg;


// -------------------------------------------------------------
void setup(void)
{
  delay(1000);
  Serial.println(F("Hello Teensy 3.6 dual CAN Test With Objects."));

  Can0.begin(500000);  
//  Can1.begin(500000);
  pinMode(13, OUTPUT);
  
  //if using enable pins on a transceiver they need to be set on
  pinMode(2, OUTPUT);
//  pinMode(35, OUTPUT);

  digitalWrite(2, HIGH);
//  digitalWrite(35, HIGH);

  msg.ext = 0;
  msg.id = 0x100;
  msg.len = 8;
  msg.buf[0] = 10;
  msg.buf[1] = 20;
  msg.buf[2] = 0;
  msg.buf[3] = 100;
  msg.buf[4] = 128;
  msg.buf[5] = 64;
  msg.buf[6] = 32;
  msg.buf[7] = 16;
}


// -------------------------------------------------------------
void loop(void)
{
Serial.println("Writing to CAN");
  msg.buf[0]++;
  Can0.write(msg);
  msg.buf[0]++;
  Can0.write(msg);
  msg.buf[0]++;
  Can0.write(msg);
  msg.buf[0]++;
  Can0.write(msg);
  msg.buf[0]++;
  Can0.write(msg);  
  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  delay(200);
  
}
