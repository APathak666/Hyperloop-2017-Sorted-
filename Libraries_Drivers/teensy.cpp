#include <kinetis.h>
#include <stdlib.h>
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <SD.h>
#include <SPI.h>
#include <Ethernet.h>
#include<FlexCAN.h>

#define Arr_size  10
#define SERIAL_BAUD_RATE 115200
#define BUFFER_SIZE 256
#ifndef __MK66FX1M0__
  #error "Teensy 3.6 with dual CAN bus is required to run this example"
#endif
static CAN_message_t msg;
static uint8_t hex[17] = "0123456789abcdef";

// Ethernet intialisation
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
// telnet defaults to port 23
EthernetServer server(23);
EthernetClient clients[4];

//CAN buffer intialisation
char buf[BUFFER_SIZE];
int sensorData;
int currTraj[2][Arr_size]={0},expTraj[2][Arr_size]={0};


// -------------------------------------------------------------
static void hexDump(uint8_t dumpLen, uint8_t *bytePtr)
{
  uint8_t working;
  while( dumpLen-- ) {
    working = *bytePtr++;
    Serial.write( hex[ working>>4 ] );
    Serial.write( hex[ working&15 ] );
  }
  Serial.write('\r');
  Serial.write('\n');
}


// pod specification struct
struct VCCanRec
{
    float acceleration_x,acceleration_y,acceleration_z;
    float pitch;
    float position_x,position_y,position_z;
    bool  pusher_attach;
    float roll;
    float velocity_x,velocity_y,velocity_z;
    float yaw;
};
struct VCCanRec vcCanRec1;

void sensSetup();
void canRecSetup();
void setup();
void dataAcquisition();  
void dataLogging();
void dataLogsetup();


void setup()
{ 
	void canRecSetup();
	void sensSetup();
	Ethernet.begin(mac, ip, myDns, gateway, subnet);
  // start listening for clients
  server.begin();
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Chat server address:");
  Serial.println(Ethernet.localIP());
} 

void canRecSetup()
{
     vcCanRec1.acceleration_x=0;
     vcCanRec1.acceleration_y=0;
     vcCanRec1.acceleration_z=0;
     vcCanRec1.pitch=0;
     vcCanRec1.position_x=0;
     vcCanRec1.position_y=0;
     vcCanRec1.position_z=0;
     vcCanRec1.pusher_attach=false;
     vcCanRec1.velocity_x=0;
     vcCanRec1.velocity_y=0;
     vcCanRec1.velocity_z=0;
}

void sensSetup()
{
    sensorData=0;
} 

void dataAcquisition()
{
 


}

/*
void dataLogsetup()
{
 	Serial.begin(9600);
	SD.begin(cs pin4);
	while(!Serial)
	{
		;
	}
	Serial.println("sd card init...");
	if(!SD.begin(4))
	{	  
		Serial.println("error in loading sd");
         	return;
	}
	else
	Serial.println("loaded");     
     
}
*/

void dataLogging()
{
	//dataLogsetup();
	String data="";
	data=String(analogRead(1/*pin to which sensor is connected*/));
	File datafile=SD.open("sensordata.txt",FILE_WRITE);
	if(!datafile)
	{
		Serial.println("error opening");
	}
	else
	{
		datafile.println(data);
		datafile.close();
	}
      
}

void getData()
{

}
// task1 implements datalogging on SD card
static void MyTask1(void* pvParameters)
{
  while(1)
  {
    Serial.println(F("Task1"));
    dataLogging();
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

// Task 3 implements Ehternet example
static void MyTask3(void* pvParameters)
{
  while(1)
  {
    // wait for a new client:
  EthernetClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {

    boolean newClient = true;
    for (byte i = 0; i < 4; i++) {
      //check whether this client refers to the same socket as one of the existing instances:
      if (clients[i] == client) {
        newClient = false;
        break;
      }
    }

    if (newClient) {
      //check which of the existing clients can be overridden:
      for (byte i = 0; i < 4; i++) {
        if (!clients[i] && clients[i] != client) {
          clients[i] = client;
          // clear out the input buffer:
          client.flush();
          Serial.println("We have a new client");
          client.print("Hello, client number: ");
          client.print(i);
          client.println();
          break;
        }
      }
    }

    if (client.available() > 0) {
      // read the bytes incoming from the client:
      char thisChar = client.read();
      // echo the bytes back to all other connected clients:
      for (byte i = 0; i < 4; i++) {
        if (clients[i] && (clients[i] != client)) {
          clients[i].write(thisChar);
        }
      }
      // echo the bytes to the server as well:
      Serial.write(thisChar);
    }
  }
  for (byte i = 0; i < 4; i++) {
    if (!(clients[i].connected())) {
      // client.stop() invalidates the internal socket-descriptor, so next use of == will allways return false;
      clients[i].stop();
    }
  }
  }
}

// Task2 implements can example
static void MyTask2(void* pvParameters)
{
  while(1)
  {    
    Serial.println(F("Task2"));
    vTaskDelay(150/portTICK_PERIOD_MS);
    int count;
  
  count = Serial.available();
  if( count > 0 )
  {
    if( count > BUFFER_SIZE )
    {
      count = BUFFER_SIZE;
    }
    Serial.readBytes( buf, count );
    Serial1.write( (uint8_t*)buf, count );
  }
  
  count = Serial1.available();
  if( count > 0 )
  {
    if( count > BUFFER_SIZE )
    {
      count = BUFFER_SIZE;
    }
    Serial1.readBytes( buf, count );
    Serial.write( (uint8_t*)buf, count );
  }
  }
CAN_message_t inMsg;
  while (Can0.available()) 
  {
    Can0.read(inMsg);
    Serial.print("CAN bus 0: "); hexDump(8, inMsg.buf);
  }
  msg.buf[0]++;
  Can1.write(msg);
  msg.buf[0]++;
  Can1.write(msg);
  msg.buf[0]++;
  Can1.write(msg);
  msg.buf[0]++;
  Can1.write(msg);
  msg.buf[0]++;
  Can1.write(msg);  
  delay(20);
}

int main()
{
	Serial.begin( SERIAL_BAUD_RATE );
	Serial1.begin( SERIAL_BAUD_RATE );
	xTaskCreate(MyTask1, "Task1", 100, NULL, 1, NULL);
	xTaskCreate(MyTask2, "Task2", 100, NULL, 2, NULL);
	xTaskCreate(MyTask3, "Task2", 100, NULL, 2, NULL);
 	void setup();

   
	return 0;
}

