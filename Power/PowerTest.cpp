/*
Task1 -> Voltage from mini
Task2 -> Temperature
Task3 -> Pressure
Task4 -> Battery ADC thermistor
Task5 -> Hall sensors ADC
Task6 -> HBridge
Task7 -> Clutch
Task8 -> Contactor
Task9 -> Stepper Motor
Task10 ->DC motor driver
*/

/*headers for freeRTOS*/
#include <kinetis.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <Arduino.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>
#include <event_groups.h>
/*sensor libraries*/
#include <FlexCAN.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <i2c_t3.h>
/*actuators libraries*/
/*FlexCAN header*/
#include <FlexCAN.h>


/*channels for ADC*/
#define CH0 0x08
#define CH1 0x09
#define CH2 0x0A
#define CH3 0x0B
#define CH4 0x0C
#define CH5 0x0D
#define CH6 0x0E
#define CH7 0x0F
/*general variables*/
#define Serial_BAUD 9600
/*sensor pins*/
#define VOLTAGEIN 31  //get Mini Serial inputs
#define CS 10 //select ADC
#define MISO 12
#define MOSI 11
#define CLK 14
/*actuator pins*/
#define HBRIDGE_E 6  //HBRIDGE enable
#define HBRIDGE_D 5  //HBRIDGE direction
#define CLUTCH 25 //clutch output control
#define CONTACTOR 24  //activate battery contactor  ??
#define STEPPERPWM 35  //stepper_motor1 - pins??
#define STEPPEREN 36  //stepper_motor1 - pins??
#define STEPPERDIR 37  //stepper_motor1 - pins??
#define STEPPERFREQ 1500 //offline analogWriteFrequency
#define STEPPERDIST 12500  //Distance (in nm)actuated in 1 pulse
/*sensor ranges*/
#define CURRMAX
#define CURRMIN
#define TEMPMIN
#define TEMPMAX
#define VOLTMAX
#define VOLTMIN
#define AMBTEMPMAX
#define AMBTEMPMIN
#define AMBPRESMAX
#define AMBPRESMIN

/*time-period for inputs*/
#define gy_tempTP 1000        //Temp Sensor
#define honey_wellTP 1000     //Pressure
#define ADCResistanceTP 10000   //(ADC TEmp)??Temp derived from Resistance
#define ADCCurrentTP 10000      //(ADC Current??)
/*steps per revolution of stepper motor*/
/*define serial for DC motor*/
#define Talk_Motor Serial2
/*CAN settings*/
//Message IDs of packets to be sent
#define HLTHACK_ID 0x03 //3  (Health Ack)                  
#define SYS_STATE_PKT_ID 0x22 //34  (System state change)
#define SSACK_ID 0x42 //66  (System State ack)
#define CANB1_ID 0xC1 //193 (CAN Broadcast Pkt1 : Voltagex4, Amb Temp, Amb Press)
#define CANB2_ID 0xC2 //194 (CAN Broadcast Pkt2 : Currentx4)
#define CANB3_ID 0xC3 //195 (CAN Broadcast Pkt3 : Tempx4)

//CAN Filters
#define FILTER_0 0x01    //Filter for Health Pkt from Comm node  (0x01)
#define FILTER_1 0x20    //Filter for System State change packets (0x21,0x23,0x24)
#define FILTER_2 0x40    //Filter for System State Ack packets (0x41,0x43,0x44) 
#define FILTER_3 0x71    //Filter for Braking Actuation Pkt from Nav: online (0x61)
#define FILTER_4 0x72    //Filter for LSD RPM Packet from Nav: online (0x62)
#define FILTER_5 0x73    //Filter for Linear Actuator Pkt from Nav: online (0x63)
#define FILTER_6 0xE0    //Filter for Linear Actuator Pkt from Nav: online (0x63)
#define FILTER_7 0x81    //Filter for Clutch Engage Pkt from Comm: offline (0x81)
#define FILTER_8 0x82    //Filter for Braking Actuation Pkt from Comm: offline (0x82)
#define FILTER_9 0x83    //Filter for LSD RPM Packet from Comm: offline (0x83)
#define FILTER_A 0x84    //Filter for Linear Actuator Pkt from Comm: offline (0x84)
#define FILTER_B 0xB0    //Filter for Linear Actuator Pkt from Comm: offline (0x84)
/*define freeRTOS handlers*/
SemaphoreHandle_t accMutex;//semaphore handles to lock complete function execution
EventGroupHandle_t grp;//eventGroup handles for ISR and their related functions
SemaphoreHandle_t PKTHDLE[12];
TaskHandle_t voltFunc=NULL;

/*define all global variables*/
volatile int system_state=0;//System state
volatile int Mini;//serial input from Mini
volatile int MiniVoltage[4]={0,0,0,0};//indivitual voltages from mini
volatile float gy_temp;//temperature sensor value          ??Change data type?
volatile float honeywell_pr;  //Pressure Sensors   
volatile float ADCResistance[4] = {0,0,0,0}; //ADC thermistor value
volatile float ADCCurrent[4] = {0,0,0,0};//ADC voltage value
volatile bool activateClutch=1;//clutch activation control variable
volatile bool directionHBridge=0;//set direction for HBridge        ??How many variables for HBridge?
volatile bool enableHBridge=0;//enable the HBridge: PWM here
volatile bool activateContactor=0;//contactor activation control variable

CAN_message_t msg;//msg in the can send buffer
CAN_message_t msgRec[12];//msg in the can Receive buffer

/*defining CAN packets*/
CAN_message_t sys_state_change;
CAN_message_t sys_state_ack;
CAN_message_t volt4Temp1Press1; //Packet1: Voltages of 4 battery packs, 1 Ambient Temperature & 1 Ambient Pressure 
CAN_message_t current4; //Packet2: Current from 4 battery packs
CAN_message_t temp4;  //Packet3: Temp from 4 battery packs

/*function definations*/
/*sensor functions*/
void voltageFunc(void * args);//arduino Mini serial input   //event bits : 1UL<<1UL
void voltageISR();
void gy_bmp_280(void *args);//temperature sensor
double adc(int8_t readAddress);//reading specified ADC channel
void adc_thermistor(void *args);  //read ADC for thermistor channels
void adc_current(void *args); //read ADc for voltage channels
/*node general functions*/
void functionTest(void *args);  //event bits : 1UL<<6UL

void CANReceive0(void *args);  //analyse data packet 0 from CAN
void CANReceive1(void *args);  //analyse data packet 1 from CAN
void CANReceive2(void *args);  //analyse SystemState packet from CAN
void CANReceive3(void *args);  //analyse data packet 3 from CAN
void CANReceive4(void *args);  //analyse data packet 4 from CAN
void CANReceive5(void *args);  //analyse data packet 5 from CAN
void CANReceive6(void *args);  //analyse data packet 6 from CAN
void CANReceive7(void *args);  //analyse data packet 7 from CAN
void CANReceive8(void *args);  //analyse data packet 8 from CAN
void CANReceive9(void *args);  //analyse data packet 9 from CAN
void CANReceiveA(void *args);  //analyse data packet A from CAN
void CANReceiveB(void *args);  //analyse data packet B from CAN
void setup();
int main();

// /*define clases to be used for DC motor driving*/

// // //----------------------------------------------------------------

class CanListener1 : public CANListener 
{
public:
  bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

CanListener1 canListener;

bool CanListener1::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
  msgRec[mailbox] = frame;
    xSemaphoreGiveFromISR(PKTHDLE[mailbox], pdFALSE);
    return true;
}

//Receiving Health Packets 
void CANReceive0(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[0], portMAX_DELAY);  
    taskDISABLE_INTERRUPTS();
    Serial1.print("health pkt received : ");
    Serial1.println(msgRec[0].buf[0]);
    taskENABLE_INTERRUPTS();
  }
}

//Receiving System State Change packet
void CANReceive1(void *args){
  uint8_t counter1=0;
  uint8_t counter2=0;
  uint8_t counter3=0;
  for (;;){
    xSemaphoreTake(PKTHDLE[1], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    Serial1.print("New system state : ");
    Serial1.println(msgRec[1].buf[0]);
    if (msgRec[1].id==0x21){
      sys_state_ack.buf[0] = 1;
      sys_state_ack.buf[1] = counter1++;
      Can0.write(sys_state_ack);
    }
    if (msgRec[1].id==0x23){
      sys_state_ack.buf[0] = 3;
      sys_state_ack.buf[1] = counter2++;
      Can0.write(sys_state_ack);
    }
    if (msgRec[1].id==0x24){
      sys_state_ack.buf[0] = 4;
      sys_state_ack.buf[1] = counter3++;
      Can0.write(sys_state_ack);
    }
    system_state = msgRec[1].buf[0];
    taskENABLE_INTERRUPTS();  //Send system state change acknowledge
  }
}

//Receiving System state change Ack
void CANReceive2(void* args){
  for (;;){
    xSemaphoreTake(PKTHDLE[2], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    if (msgRec[2].id==0x41)
      Serial1.print("Received system state change ack from Nav Node");
    if (msgRec[2].id==0x43)
      Serial1.print("Received system state change ack from Control Node");
    if (msgRec[2].id==0x44)
      Serial1.print("Received system state change ack from Comm Node"); 
    taskENABLE_INTERRUPTS();       
  }
}

//Receiving Braking Actuation from Nav node when Online
void CANReceive3(void *args){
  for (;;){
    //xSemaphoreTake(PKTHDLE[3], portMAX_DELAY);
    vTaskSuspend(NULL);
    taskDISABLE_INTERRUPTS();
    digitalWriteFast(STEPPEREN, LOW);
    delayMicroseconds(5);
	digitalWriteFast(STEPPERDIR, LOW);
	delayMicroseconds(5);
    analogWriteFrequency(STEPPERPWM, (uint16_t) *(msgRec[3].buf)); 
    taskENABLE_INTERRUPTS();    
  }
}


//Receiving LSD actuation from Nav node when Online
void CANReceive4(void *args){
    uint8_t level;
    Talk_Motor.begin(9600);
    taskDISABLE_INTERRUPTS();
    Talk_Motor.write(0);
    Talk_Motor.write(128);
    taskENABLE_INTERRUPTS();
    Serial1.println("Low speed Drive motor initialised correctly");
  
    for (;;){
      xSemaphoreTake(PKTHDLE[4], portMAX_DELAY);
      taskDISABLE_INTERRUPTS();
      level=msgRec[4].buf[0];
      taskENABLE_INTERRUPTS();
    if(level>0 && level<=63){
      taskDISABLE_INTERRUPTS();
      Talk_Motor.write(192+level);    //Move Forward, Right motor - 192 (11000000) to 255 (11111111)
      Talk_Motor.write(64+level);     //Left motor - 64 (01000000) to 127 (01111111)
      taskENABLE_INTERRUPTS();
    }
    else if(level==0){
      taskDISABLE_INTERRUPTS();
      Talk_Motor.write(0);            //Left motor stop - 0 or 64
      Talk_Motor.write(128);          //Right motor stop - 128 or 192
      taskENABLE_INTERRUPTS();
    }
    else
      Serial1.println("Value not permitted");
  }
}

//Receiving Linear actuator pkt from Nav node when Online
void CANReceive5(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[5], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    directionHBridge = (msgRec[5].buf[0] && 0x0F);
    enableHBridge = (msgRec[5].buf[0] && 0xF0) >> 4;
    digitalWriteFast(HBRIDGE_E, enableHBridge);  //HBRIDGE enable (inverted)
    digitalWriteFast(HBRIDGE_D, directionHBridge);  //HBRIDGE direction (inverted)
    taskENABLE_INTERRUPTS();
  }
}

//Receiving signal to Engage/Disengage Clutch
void CANReceive6(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[6], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    activateClutch = msgRec[6].buf[0];
    digitalWrite(CLUTCH,activateClutch);
    taskENABLE_INTERRUPTS();
  }
}

//Receiving signal for Emergency Braking  
void CANReceive7(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[7], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    activateClutch = 0;
    digitalWrite(CLUTCH,0);
    taskENABLE_INTERRUPTS();  
  }
}

//Receiving Braking Actuation from Comm node when Offline
void CANReceive8(void *args){
  for (;;){
    //xSemaphoreTake(PKTHDLE[8], portMAX_DELAY);
    vTaskSuspend(NULL);
    taskDISABLE_INTERRUPTS();
    /*
    Serial1.print("Value of Braking Actuation : ");
    for (int i=0; i<8 ;i++)
      Serial1.println(msgRec[8].buf[i]);
    */
    //Add stepper.cpp ka code
    //Braking in mm (based on distance moved in one step)
    digitalWriteFast(STEPPEREN, LOW);
    delayMicroseconds(5);
	digitalWriteFast(STEPPERDIR, LOW);
 	delayMicroseconds(5);
    int numPulses = (1000000 * msgRec[8].buf[0])/STEPPERDIST;
    analogWriteFrequency(STEPPERPWM,STEPPERFREQ); 
    delayMicroseconds((1000000 * numPulses)/STEPPERFREQ);
    digitalWriteFast(STEPPERPWM, LOW);
    taskENABLE_INTERRUPTS();
  }
}


//Receiving LSD actuation from Comm node when Offline
void CANReceive9(void *args){
  uint8_t level;
  Talk_Motor.begin(9600);
  taskDISABLE_INTERRUPTS();
  Talk_Motor.write(0);
  Talk_Motor.write(128);
  taskENABLE_INTERRUPTS();
  Serial1.println("Low speed Drive motor initialised correctly");
  for (;;){
    xSemaphoreTake(PKTHDLE[9], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    level=msgRec[9].buf[0];
    taskENABLE_INTERRUPTS();
    if(level>0 && level<=63){
      taskDISABLE_INTERRUPTS();
      Talk_Motor.write(192+level);    //Move Forward, Right motor - 192 (11000000) to 255 (11111111)
      Talk_Motor.write(64+level);     //Left motor - 64 (01000000) to 127 (01111111)
      taskENABLE_INTERRUPTS();
    }
    else if(level==0){
      taskDISABLE_INTERRUPTS();
      Talk_Motor.write(0);            //Left motor stop - 0 or 64
      Talk_Motor.write(128);          //Right motor stop - 128 or 192
      taskENABLE_INTERRUPTS();
    }
    else
      Serial1.println("Value not permitted");
  }
}

//Receiving Linear actuator pkt from Comm node when Offline
void CANReceiveA(void *args){
  for (;;){
    xSemaphoreTake(PKTHDLE[10], portMAX_DELAY);
    taskDISABLE_INTERRUPTS();
    directionHBridge = (msgRec[10].buf[0] && 0x0F);
    enableHBridge = (msgRec[10].buf[0] && 0xF0) >> 4;
    digitalWriteFast(HBRIDGE_E, enableHBridge);  //HBRIDGE enable (inverted)
    digitalWriteFast(HBRIDGE_D, directionHBridge);  //HBRIDGE direction (inverted)
    taskENABLE_INTERRUPTS();
  }
}

void CANReceiveB(void *args){
  for (;;){
    // xSemaphoreTake(PKTHDLE[11], portMAX_DELAY);
    vTaskSuspend(NULL);
    taskDISABLE_INTERRUPTS();
    activateContactor = msgRec[11].buf[0];
    digitalWriteFast(CONTACTOR, activateContactor);  
    taskENABLE_INTERRUPTS();
  }
}


void voltageFunc(void * args){
  while(1){
    //xEventGroupWaitBits(grp,1UL<<1UL,pdTRUE,pdTRUE,portMAX_DELAY);
    ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
    if(Mini>=100){
      MiniVoltage[2]=Mini%10;
      Mini/=10;
      MiniVoltage[3]=Mini%10;
    }
    else{
      MiniVoltage[0]=Mini%10;
      Mini/=10;
      MiniVoltage[1]=Mini%10;
    }
    
    Serial1.print(" mini battery 1=");
    Serial1.print(MiniVoltage[0]);
    Serial1.print(" mini battery 2=");
    Serial1.print(MiniVoltage[1]);
    Serial1.print(" mini battery 3=");
    Serial1.print(MiniVoltage[2]);
    Serial1.print(" mini battery 4=");
    Serial1.println(MiniVoltage[3]);      
  }
}

//ISR raised when data is available on serial4 from arduino Mini
void voltageISR(){
  BaseType_t xHigherPriorityTaskWoken=pdFALSE;
  if(Serial4.available()>0){
    Mini=Serial4.read();    
    //xEventGroupSetBitsFromISR(grp, 1UL << 1UL, &xHigherPriorityTaskWoken);
    vTaskNotifyGiveFromISR(voltFunc, &xHigherPriorityTaskWoken);
  }
}

//get temperature values from gy_bmp_280
void gy_bmp_280(void *args)
{
  Adafruit_BMP280 bme;
  while(1){
    //Adafruit_BMP280 bme;
    //while(!Serial.available());
    if (!bme.begin()){
      Serial1.println("Could not find a valid BMP280 sensor, check wiring!");
      delayMicroseconds(2000);
    }
    else{
      xSemaphoreTake( accMutex,portMAX_DELAY);
      gy_temp=bme.readTemperature();
      // Serial1.print("gy_temp =");
      // Serial1.println(gy_temp);
      //if (gy_temp < AMBTEMPMIN or gy_temp > AMBTEMPMAX)

      xSemaphoreGive( accMutex );
    }  
    vTaskDelay(pdMS_TO_TICKS(gy_tempTP));  
  }
}


/*ADC function to read values on the specified ADC channel*/
double adc(int8_t readAddress){
  double val;
  const double vRef = 5;
  int   adc_reading = 0;
  byte dataMSB =0;
  byte dataLSB =0;
  byte JUNK = 0;
  SPISettings adc(3600000,MSBFIRST,SPI_MODE0);
  SPI.begin();
  pinMode(CS,OUTPUT);
  digitalWrite(CS,LOW);
  digitalWrite(CS,HIGH);
  SPI.beginTransaction(adc);
  digitalWrite(CS,LOW);

  SPI.beginTransaction (adc);
  digitalWrite         (CS, LOW);
  SPI.transfer         (0x01);                                 // Start Bit
  dataMSB =            SPI.transfer(readAddress << 4) & 0x03;  // Send readAddress and receive MSB data, masked to two bits
  dataLSB =            SPI.transfer(JUNK);                     // Push junk data and get LSB byte return
  digitalWrite         (CS, HIGH);
  SPI.endTransaction   ();
   
  adc_reading = dataMSB << 8 | dataLSB;
  val = adc_reading*vRef/1024;
  return val;
}

/*get thermistor values from ADC
*values are inputted from CH0 to CH3
*these values are converted from 0-5 to 0-1024 range
*/
void adc_thermistor(void *args)
{
  while(1){
    xSemaphoreTake( accMutex,portMAX_DELAY);
    ADCResistance[0]=adc(CH0);
    ADCResistance[1]=adc(CH1);
    ADCResistance[2]=adc(CH2);
    ADCResistance[3]=adc(CH3);
    
    Serial1.print(" temperature 1=");
    Serial1.print(ADCResistance[0]);
    Serial1.print(" temperature 2=");
    Serial1.print(ADCResistance[1]);
    Serial1.print(" temperature 3=");
    Serial1.print(ADCResistance[2]);
    Serial1.print(" temperature 4=");
    Serial1.println(ADCResistance[3]);
    
    xSemaphoreGive( accMutex );
    vTaskDelay(pdMS_TO_TICKS(ADCResistanceTP));
  }
}

/*get hall sensors voltage values from ADC
*values are inputted from CH4 to CH7
*these values are converted from 0-5 to 0-1024 range
*/
void adc_current(void *args)
{
  while(1){
    xSemaphoreTake( accMutex,portMAX_DELAY);
    ADCCurrent[0]=adc(CH4);
    ADCCurrent[1]=adc(CH5);
    ADCCurrent[2]=adc(CH6);
    ADCCurrent[3]=adc(CH7);
    
    Serial1.print(" current 1=");
    Serial1.print(ADCCurrent[0]);
    Serial1.print(" current 2=");
    Serial1.print(ADCCurrent[1]);
    Serial1.print(" current 3=");
    Serial1.print(ADCCurrent[2]);
    Serial1.print(" current 4=");
    Serial1.println(ADCCurrent[3]);
    
    xSemaphoreGive( accMutex );
    vTaskDelay(pdMS_TO_TICKS(ADCCurrentTP));
  }
}

//HoneyWell, Stepper, Cytron Functions yet to be written

//setup the pinModes and create tasks
void setup(){
  //Serial Setup
  Serial1.begin(Serial_BAUD);
  Serial4.begin(Serial_BAUD);
  Serial4.begin(Serial_BAUD);
  //CAN Setup
  Can0.begin(1000000);
  Can0.attachObj(&canListener);
  canListener.attachMBHandler(0);
  canListener.attachMBHandler(1);
  canListener.attachMBHandler(2);
  canListener.attachMBHandler(3);
  canListener.attachMBHandler(4);
  canListener.attachMBHandler(5);
  canListener.attachMBHandler(6);

  sys_state_change.ext = 0;
  sys_state_change.id = 0x22; //5
  sys_state_change.len = 8;
  sys_state_ack.ext = 0;
  sys_state_ack.id = 0x42; //9
  sys_state_ack.len = 8;
  volt4Temp1Press1.ext = 0;
  volt4Temp1Press1.id = 0xC1;  //20
  volt4Temp1Press1.len = 8;
  current4.ext = 0;
  current4.id = 0xC2;  //21;
  current4.len = 8; 
  temp4.ext = 0;
  temp4.id = 0xC3;  //22
  temp4.len = 8;  


  CAN_filter_t filter[12];
  // Framehandler invoked only for id 560
  filter[0].flags.extended=0;
  filter[0].flags.remote=0;
  filter[0].flags.reserved=0;
  filter[0].id=FILTER_0;
  Can0.setFilter(filter[0],0);
  Can0.setMask(0x1FFFFFF8,0);

  filter[1] .flags.extended=0;
  filter[1].flags.remote=0;
  filter[1].flags.reserved=0;
  filter[1].id=FILTER_1;
  Can0.setFilter(filter[1],1);
  Can0.setMask(0x1FFFFFF8,1);   

  filter[2].flags.extended=0;
  filter[2].flags.remote=0;
  filter[2].flags.reserved=0;
  filter[2].id=FILTER_2;
  Can0.setFilter(filter[2],2);
  Can0.setMask(0x1FFFFFF8,2);

  filter[3].flags.extended=0;
  filter[3].flags.remote=0;
  filter[3].flags.reserved=0;
  filter[3].id=FILTER_3;
  Can0.setFilter(filter[3],3);
  Can0.setMask(0x1FFFFFFF,3);

  filter[4].flags.extended=0;
  filter[4].flags.remote=0;
  filter[4].flags.reserved=0;
  filter[4].id=FILTER_4;
  Can0.setFilter(filter[4],4);
  Can0.setMask(0x1FFFFFFF,4);

  filter[5].flags.extended=0;
  filter[5].flags.remote=0;
  filter[5].flags.reserved=0;
  filter[5].id=FILTER_5;
  Can0.setFilter(filter[5],5);
  Can0.setMask(0x1FFFFFFF,5);
  
  filter[6].flags.extended=0;
  filter[6].flags.remote=0;
  filter[6].flags.reserved=0;
  filter[6].id=FILTER_6;
  Can0.setFilter(filter[6],6);
  Can0.setMask(0x1FFFFFFF,6);

  filter[7].flags.extended=0;
  filter[7].flags.remote=0;
  filter[7].flags.reserved=0;
  filter[7].id=FILTER_7;
  Can0.setFilter(filter[7],7);
  Can0.setMask(0x1FFFFFFF,7);

  filter[8].flags.extended=0;
  filter[8].flags.remote=0;
  filter[8].flags.reserved=0;
  filter[8].id=FILTER_8;
  Can0.setFilter(filter[8],8);
  Can0.setMask(0x1FFFFFFF,8);

  filter[9].flags.extended=0;
  filter[9].flags.remote=0;
  filter[9].flags.reserved=0;
  filter[9].id=FILTER_9;
  Can0.setFilter(filter[9],9);
  Can0.setMask(0x1FFFFFFF,9); 

  filter[10].flags.extended=0;
  filter[10].flags.remote=0;
  filter[10].flags.reserved=0;
  filter[10].id=FILTER_A;
  Can0.setFilter(filter[10],10);
  Can0.setMask(0x1FFFFFFF,9); 

  filter[11].flags.extended=0;
  filter[11].flags.remote=0;
  filter[11].flags.reserved=0;
  filter[11].id=FILTER_B;
  Can0.setFilter(filter[11],11);
  Can0.setMask(0x1FFFFFFF,11); 

  pinMode(HBRIDGE_E,OUTPUT);
  pinMode(HBRIDGE_D,OUTPUT);
  pinMode(CLUTCH,OUTPUT);
  pinMode(CONTACTOR,OUTPUT);
  pinMode(STEPPEREN, OUTPUT);
  pinMode(STEPPERDIR, OUTPUT);
  pinMode(STEPPERPWM, OUTPUT);
  digitalWrite(CONTACTOR, HIGH);
  delayMicroseconds(1000000);
  digitalWrite(CONTACTOR, LOW);
  delayMicroseconds(1000000);
  digitalWrite(CONTACTOR, HIGH);
  
  pinMode(13,OUTPUT);
  digitalWriteFast(13,HIGH);
  delayMicroseconds(1000000);
  digitalWriteFast(13,LOW);	
  delayMicroseconds(1000000);					//
  digitalWriteFast(13,HIGH);

  digitalWriteFast(STEPPEREN, HIGH);
	delayMicroseconds(5);
digitalWriteFast(STEPPERDIR, HIGH);
delayMicroseconds(5);

int freq = 1500;
analogWriteResolution(15);
analogWriteFrequency(STEPPERPWM, 1700);
analogWrite(STEPPERPWM, 16384);

for (int i=0; i<45; i++){
	freq += 40;
	analogWriteFrequency(STEPPERPWM, freq);
	delayMicroseconds(20000);
}
digitalWriteFast(STEPPERDIR, LOW);
delayMicroseconds(5);
//digitalWriteFast(STEPPEREN, HIGH);
delayMicroseconds(5);
analogWrite(STEPPERPWM, 16384);
//analogWriteFrequency(STEPPERPWM, 0);
//analogWrite(STEPPERPWM, 16384);
for (int i=0; i<45; i++){
	freq -= 40;
	analogWriteFrequency(STEPPERPWM, freq);
	delayMicroseconds(20000);
}
analogWrite(STEPPERPWM, 16384);
delayMicroseconds(1000000);
analogWrite(STEPPERPWM, 0);

 

  //GPIOC_PTOR = 1 << 5;
  // delayMicroseconds(2000000);
  // GPIOC_PTOR = 1 << 5;

  //xTaskCreate(voltageFunc, "funcVoltage", configMINIMAL_STACK_SIZE, NULL, 4, &voltFunc);
  //xTaskCreate(gy_bmp_280, "gy_bmp_280", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
  //xTaskCreate(honey_well, "honey_well", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  //xTaskCreate(adc_thermistor, "adc_thermistor", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
  //xTaskCreate(adc_current, "adc_current", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
  //xTaskCreate(functionTest, "functionTest", configMINIMAL_STACK_SIZE, NULL, 6, NULL);
  //xTaskCreate(CANSend, "CANSend", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
  xTaskCreate(CANReceive0, "CANReceive0", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive1, "CANReceive1", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive2, "CANReceive2", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive3, "CANReceive3", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive4, "CANReceive4", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive5, "CANReceive5", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive6, "CANReceive6", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive7, "CANReceive7", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive8, "CANReceive8", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceive9, "CANReceive9", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceiveA, "CANReceiveA", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  xTaskCreate(CANReceiveB, "CANReceiveB", configMINIMAL_STACK_SIZE, NULL, 8, NULL);

  attachInterrupt(VOLTAGEIN,voltageISR,CHANGE);
  grp=xEventGroupCreate();
  accMutex = xSemaphoreCreateMutex();
  PKTHDLE[0] = xSemaphoreCreateMutex();   //Mutexes to redirect control from FrameHandler to CANRcv tasks
  PKTHDLE[1] = xSemaphoreCreateMutex();    
  PKTHDLE[2] = xSemaphoreCreateMutex();
  PKTHDLE[3] = xSemaphoreCreateMutex();
  PKTHDLE[4] = xSemaphoreCreateMutex();
  PKTHDLE[5] = xSemaphoreCreateMutex();
  PKTHDLE[6] = xSemaphoreCreateMutex();
  PKTHDLE[7] = xSemaphoreCreateMutex();
  PKTHDLE[8] = xSemaphoreCreateMutex();
  PKTHDLE[9] = xSemaphoreCreateMutex();
  PKTHDLE[10] = xSemaphoreCreateMutex();
  PKTHDLE[11] = xSemaphoreCreateMutex();
}

int main(){
  setup();
  //while(1){}
  Serial1.println("main started");
  vTaskStartScheduler();
  for(;;);  
  return 0;
}