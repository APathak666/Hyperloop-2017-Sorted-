#include <kinetis.h>
#include <stdlib.h>
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include "SD/src/SD.h"



#define Arr_size  10
/*
****************************************
*****************VARIABLES AND STRUCT 
*****************
****************************************
*/


int sensorData;

/* 
 *currTraj->currentTrajectory
 * expTraj->expectedTrajectory
*/
int currTraj[2][Arr_size]={0},expTraj[2][Arr_size]={0};

/*
*VCCanRec->VehicleController_CanReceive
*/
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
  /*Global obj of struct */
  struct VCCanRec vcCanRec1;







/*
****************************************
*************
PROTOTYPES------SETUP FUNCTIONS 
*************
****************************************
*/

//sensor setup func
void sensSetup();
//VCCCanRec strucr obj setup func
void canRecSetup();
//setup func to setup all global variables
void setup();







/*
****************************************
*************
PROTOTYPES------SENSOR FUNC
*************
****************************************
*/

void dataAcquisition();  
void dataLogging();
//func to setup the sd card and serial port for the data logging
void dataLogsetup();










/*
****************************************
***********MAIN FUNCTION
***********
****************************************
*/
int main() {
 setup();
   
  return 0;
}




 


/*
****************************************
*************
DEFINITIONS------SETUP FUNCTIONS 
*************
****************************************
*/
void setup()
  {  void canRecSetup();
     void sensSetup();

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
/*
****************************************
*************
DEFINITIONS------SENSOR FUNCTION
*************
****************************************
*/
void dataAcquisition()
  {
    //sensorData=//func which returns the dist. data


  }


void dataLogsetup()
  {
     Serial.begin(9600);
     SD.begin(/*cs pin*/4);
 /*
  ***********************************************************
  ***************ERROR MSSG***********************

 while(!Serial)
  {;
    }
    Serial.println("sd card init...");
    if(!SD.begin(4))
         {  Serial.println("error in loading sd");
         return;}
    else
    Serial.println("loaded");     
     */     
  }


void dataLogging()
{

   dataLogsetup();

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

