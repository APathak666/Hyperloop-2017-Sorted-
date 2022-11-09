#include <kinetis.h>
#include <stdlib.h>
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <SD.h>
#include <SPI.h>

#include<FlexCAN.h>

#define Arr_size  10
#define SERIAL_BAUD_RATE 115200
#define BUFFER_SIZE 256
#ifndef __MK66FX1M0__
  #error "Teensy 3.6 with dual CAN bus is required to run this example"
#endif

//CAN buffer intialisation
char buf[BUFFER_SIZE];
int sensorData;
int currTraj[2][Arr_size]={0},expTraj[2][Arr_size]={0};

File myFile;
const int chipSelect = 4;
// -------------------------------------------------------------

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
void getData();


void setup()
{ 
	 Serial.begin(9600);
   while (!Serial) {
    ; 
  }


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



void dataLogging()
{
	myFile = SD.open("test.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
	// close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void getData()
{
	myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");
    
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
    	Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
  	// if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
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


// Task2 implements can example
static void MyTask2(void* pvParameters)
{
  while(1)
  {
    Serial.println(F("Task2"));
    getData();
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

int main()
{
	Serial.begin( SERIAL_BAUD_RATE );
	Serial1.begin( SERIAL_BAUD_RATE );
	xTaskCreate(MyTask1, "Task1", 100, NULL, 1, NULL);
	xTaskCreate(MyTask2, "Task2", 100, NULL, 2, NULL);
	
 	void setup();

   
	return 0;
}

