/* 

Task 1 : Temperature Sensor

Task 2 : Pressure Sensor

Task 3 : LTS

Task 4 : CAN send and write data

Task 5 : CAN receive

Task 6 : Data Logging

*/

#include <kinetis.h>
#include <stdlib.h>
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <FlexCAN.h>
#include <queue.h>
#include <timers.h>
#include <usb_dev.h>
#include <SD.h>
#include <SPI.h>
#include <stdlib.h>
#include <semphr.h>
#include <i2c_t3.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>


#define Display Serial1
#define Serial_BAUD 9600
#define CAN_BAUD 1000000
#define TempMin 
#define TempMax
#define PressureMin 
#define PressureMax
#define DataLogTimerPeriod 1000  
#define DEBUG_MODE 1
#define PressureDelay 1
#define TemperatureDelay 1
#define LTSPIN1
#define LTSPIN2
#define LTSPIN3
#define LTSPIN4

#define HIGH_V 3.2  
#define LOW_V 0.64
#define HIGH_D 8.5
#define LOW_D 3.5
//pins

//
const int chipSelect = BUILTIN_SDCARD;
bool initialise_status =1;
volatile float global_pressure = 0;
volatile uint8_t global_temperature;
volatile float lts_1;
volatile float lts_2;
volatile float lts_3;
volatile float lts_4;

String dataString="";

TimerHandle_t dataLogTimer; 
File dataFile;
SemaphoreHandle_t accMutex;

//CAN message Packets
CAN_message_t sys_state_change;
CAN_message_t sys_state_ack;
CAN_message_t health_ack;
CAN_message_t lts_packet1;
CAN_message_t lts_packet2;
CAN_message_t temp_pressure;

#define STATECHNGID 0x23
#define SYSSTATEACK 0x43
#define TEMPPRSR    0x31
#define LTSB12      0x32
#define LTSB34      0x33
#define EMGNCY      0xE0
#define CAN_ACK_FAILURE_PERIOD 100

// CAN
class CanListener1 : public CANListener 

{

public:

  bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something

};

CanListener1 canListener1;

bool CanListener1::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)

{
  
  return true;
}


/*
Defining Tasks here:
*/


//get temperature values from gy_bmp_280
void gy_bmp_280(void *args){
  Adafruit_BMP280 bme;
  uint8_t local_temperature = 0;
  if (!bme.begin()){
    Serial1.println("Could not find a valid BMP280 sensor, check wiring!");
    while(1);
  }
  
  for(;;){
    local_temperature = bme.readTemperature();
    xSemaphoreTake( accMutex,portMAX_DELAY);
    global_temperature = local_temperature;
    xSemaphoreGive( accMutex );
    Serial1.print("Temperature = ");
    Serial1.print(local_temperature);
    Serial1.println(" *C");  
    vTaskDelay(pdMS_TO_TICKS(TEMP_DELAY));  
  }
}



void pressure(void* args){
  Wire.begin();
  Wire.setClock(400000L);
  uint16_t out = 0;
  float local_pressure =0 ;
  uint8_t address1 =0x28;
  size_t len1 = 2;
  uint32_t timeout_P =200;

  for(;;){ 
    Wire.beginTransmission(address1);
    Wire.endTransmission(timeout_P);
    Wire.requestFrom(address1,len1);
    
    while(Wire.available()){ 
      out= Wire.read();
      out = out<<8;   //We get the MSB first. Left shift the MSB by 8 bits and OR it with the new data
      out = out|Wire.read();
    }

     local_pressure =(out - 0x0666)*1.6/(0x3999 - 0x0666);
      xSemaphoreTake( accMutex,portMAX_DELAY);
      global_pressure = local_pressure;
      xSemaphoreGive( accMutex );
      Serial1.print("Pressure: ");
      Serial1.println(local_pressure);

    vTaskDelay(pdMS_TO_TICKS(PRESSURE_DELAY));
  }

}

void lts1 (void* args){

  float distance=0;
  float v = 0;

  pinMode(LTSPIN1,INPUT);
  analogReadResolution(16);
  
  for(;;){ 
    v = analogRead(LTSPIN1)*3.3/65535;
    Serial1.print("LTS1 Voltage");
    Serial1.println(v);
    distance = (v-LOW_V)/(HIGH_V - LOW_V) *(HIGH_D - LOW_D) + LOW_D;
    Serial1.print("LTS1 Distance");
    Serial1.println(distance,4);
    xSemaphoreTake( accMutex,portMAX_DELAY);
    lts_1 = distance;
    xSemaphoreGive( accMutex );
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void lts2 (void* args){

  float distance=0;
  float v = 0;

  pinMode(LTSPIN2,INPUT);
  analogReadResolution(16);
  
  for(;;){ 
    v = analogRead(LTSPIN2)*3.3/65535;
    Serial1.print("LTS2 Voltage");
    Serial1.println(v);
    distance = (v-LOW_V)/(HIGH_V - LOW_V) *(HIGH_D - LOW_D) + LOW_D;
    Serial1.print("LTS2 Distance");
    Serial1.println(distance,4);
    xSemaphoreTake( accMutex,portMAX_DELAY);
    lts_2 = distance;
    xSemaphoreGive( accMutex );
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void lts3 (void* args){

  float distance=0;
  float v = 0;

  pinMode(LTSPIN3,INPUT);
  analogReadResolution(16);
  
  for(;;){ 
    v = analogRead(LTSPIN4)*3.3/65535;
    Serial1.print("LTS3 Voltage");
    Serial1.println(v);
    distance = (v-LOW_V)/(HIGH_V - LOW_V) *(HIGH_D - LOW_D) + LOW_D;
    Serial1.print("LTS3 Distance");
    Serial1.println(distance,4);
    xSemaphoreTake( accMutex,portMAX_DELAY);
    lts_3 = distance;
    xSemaphoreGive( accMutex );
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void lts4 (void* args){

  float distance=0;
  float v = 0;

  pinMode(LTSPIN4,INPUT);
  analogReadResolution(16);
  
  for(;;){ 
    v = analogRead(LTSPIN4)*3.3/65535;
    Serial1.print("LTS4 Voltage");
    Serial1.println(v);
    distance = (v-LOW_V)/(HIGH_V - LOW_V) *(HIGH_D - LOW_D) + LOW_D;
    Serial1.print("LTS4 Distance");
    Serial1.println(distance,4);
    xSemaphoreTake( accMutex,portMAX_DELAY);
    lts_4 = distance;
    xSemaphoreGive( accMutex );
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void dataLogging(void *args){
  volatile float local_pressure = 0;
  volatile uint8_t local_temperature;
  volatile float local_lts_1;
  volatile float local_lts_2;
  volatile float local_lts_3;
  volatile float local_lts_4;

  if (dataFile) {
    Display.print("Waiting for mutex\r\n");
    xSemaphoreTake( accMutex, portMAX_DELAY );
    local_temperature = global_temperature;
    local_pressure = global_temperature;
    local_lts_4 = lts_4;
    local_lts_3 = lts_3;
    local_lts_2 = lts_2;
    local_lts_1 = lts_1;
    xSemaphoreGive( accMutex );
    dataFile.print(xTaskGetTickCount());
    dataFile.print(": \r\n");
    dataFile.print("Temperature: ");
    dataFile.print(global_temperature);
    dataFile.print("\r\n");
    dataFile.print("Pressure: ");
    dataFile.print(global_pressure);
    dataFile.print("\r\n");
    dataFile.print("LTS1: ");
    dataFile.print(local_lts_1);
    dataFile.print("\r\n");
    dataFile.print("LTS2: ");
    dataFile.print(lts_2);
    dataFile.print("\r\n");
    dataFile.print("LTS3: ");
    dataFile.print(lts_3);
    dataFile.print("\r\n");
    dataFile.print("LTS4: ");
    dataFile.print(lts4);
    dataFile.flush();

    *lts_packet1.buf = local_lts_1;
    *(lts_packet1.buf+4) = local_lts_2;
    *lts_packet2.buf = local_lts_3;
    *(lts_packet2.buf+4) = local_lts_4;
    *temp_pressure = local_pressure;
    *(temp_pressure+4) = local_temperature;
    Can0.write(lts_packet1);
    Can0.write(lts_packet2);
    Can0.write(temp_pressure);
 
  }
  else{
    Display.println("Data Logging Error");
  }
  dataString = "";
  vTaskDelay(pdMS_TO_TICKS(DataLogTimerPeriod));
}

/*

Initializing Everything

*/


void init_can(){
    Can0.begin(CAN_BAUD);
    Can0.attachObj(&canListener1);
    canListener1.attachGeneralHandler();
    sys_state_change.ext = 0;
    sys_state_change.id = STATECHNGID; 
    sys_state_change.len = 1;
    
    sys_state_ack.ext = 0;
    sys_state_ack.id = SYSSTATEACK; 
    sys_state_ack.len = 1;
    sys_state_ack.buf[0] = 3; //Verify Control Node Ack Packet number

    lts_packet1.ext = 0;
    lts_packet1.id = LTSB12; 
    lts_packet1.len = 8;
    
    lts_packet2.ext = 0;
    lts_packet2.id = LTSB34; 
    lts_packet2.len = 8;
    
    temp_pressure.ext = 0;
    temp_pressure.id = TEMPPRSR; 
    temp_pressure.len = 8;
    
    
    #ifdef DEBUG_MODE
      Display.println("Navigation: CAN initialised");
      Display.flush();
    #endif
}


void init_sdcard(){
  if (SD.begin(chipSelect)) {
    Display.println("Controller: SD card initialised");
  } 
  else{
    Display.println("Controller: SD card not initialised"); 
    initialise_status=0;
  }
   dataFile = SD.open("datalog.txt", FILE_WRITE);    
   if (dataFile) {
       Display.println("Data Logging file ready");
   }
   else{
     Display.println("Unable to open file for Data logging");
     initialise_status=0;
   }
   Display.flush();
}


int main(void) {

/* 
Controller Initialisation Starts
*/

  Display.begin(9600);
  Display.println("Starting initialisation");
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delayMicroseconds(1000000);
  digitalWrite(13, LOW);
  delayMicroseconds(1000000);

  init_can();
  init_sdcard();
//  init_software_timer();
//  init_lts();

  accMutex = xSemaphoreCreateMutex();

  if(initialise_status!=1){
    Display.println("Controller initialisation Failed. Can't proceed");
    while(1);
  }
  else{
    Display.println("Controller Initialisation successful");
  }


  //Creating Tasks Lists
  xTaskCreate(gy_bmp_280, "gy_bmp_280", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
  xTaskCreate(honey_well, "honey_well", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
  xTaskCreate(lts1, "LaserT1", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  xTaskCreate(lts2, "LaserT2", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  xTaskCreate(lts3, "LaserT3", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  xTaskCreate(lts4, "LaserT4", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

  Display.println("Scheduler Starting");

  vTaskStartScheduler();
  for(;;);  
  return 0;

}
