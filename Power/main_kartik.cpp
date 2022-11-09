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
//#include <Stepper.h>
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
#define CLK 13
/*actuator pins*/
#define HBRIDGE_E 15  //HBRIDGE enable
#define HBRIDGE_D 16  //HBRIDGE direction
#define CLUTCH 17 //clutch output control
#define CONTACTOR 18  //activate battery contactor  ??
#define STEPPER11 19  //stepper_motor1 - pins??
#define STEPPER12 20  // ??
#define STEPPER13 21  // ??
#define STEPPER14 22  // ??
/*time-period for inputs*/
#define TEMP_DELAY 1000        //Temp Sensor
#define PRESSURE_DELAY 1000     //Pressure
#define ADC_CURRENT_TEMPERATURE_DELAY 1000   
#define CAN_DELAY 1000  //delay in MS for sending normal sensor data
#define ADCCurrentTP 10000 
#define LV_DELAY 1000     //(ADC Current??)
/*steps per revolution of stepper motor*/
#define STEPPER_REV 200
#define STEPPER_SPEED 60  //STEPPER SPEED IN RPM
/*define serial for DC motor*/
#define Talk_Motor Serial2
/*CAN settings*/
#define CAN_ID1 0x01  //CAN ID for packet 1 
#define CAN_ID2 0x02  //CAN ID for packet 2
#define CAN_ID3 0x03  //CAN ID for packet 3
#define TEST_ID 0x11  //CAN ID for functional tests
#define CAN_FILTER_NAV 0x100 //CAN FILTER ID for data from Nav node
#define CAN_FILTER_COMM 0x100 //CAN FILTER ID for data from Comm node 
#define FILTER1 0x11
#define FILTER2 0x12
#define FILTER3 0x13
#define FILTER4 0x14
#define FILTER5 0x15
#define FILTER6 0x16
#define FILTER7 0x17
#define ADC_OFFSET_CURRENT 30
#define R25 10000
#define BETA 3428

/*define freeRTOS handlers*/
SemaphoreHandle_t accMutex;//semaphore handles to lock complete function execution
EventGroupHandle_t grp;//eventGroup handles for ISR and their related functions
SemaphoreHandle_t PKTHDLE1, PKTHDLE2, PKTHDLE3, PKTHDLE4, PKTHDLE5, PKTHDLE6, PKTHDLE7;
TaskHandle_t voltFunc=NULL;

/*define all global variables*/
volatile int system_state=0;//System state
SPISettings adc(3600000,MSBFIRST,SPI_MODE0);
volatile int Mini;//serial input from Mini
volatile int MiniVoltage[4]={0,0,0,0};          //indivitual voltages from mini   
volatile bool activateClutch=1;                 //clutch activation control variable
volatile bool directionHBridge=0;               //set direction for HBridge        ??How many variables for HBridge?
volatile bool enableHBridge=0;                  //enable the HBridge: PWM here
volatile bool activateContactor=0;              //contactor activation control variable
volatile float global_pressure = 0;
volatile uint8_t global_temperature=0;          //temperature sensor value
volatile uint16_t ADCthermistor_global[4] = {0,0,0,0};    //ADC thermistor value
volatile uint16_t ADCcurrent_global[4] = {0,0,0,0};       //ADC current value
CAN_message_t msg;//msg in the can send buffer
CAN_message_t msgRec;//msg in the can Receive buffer

/*defining CAN packets*/
CAN_message_t sys_state_change;
CAN_message_t sys_state_ack;
CAN_message_t health_ack;
CAN_message_t volt4Temp1Press1; //Packet1: Voltages of 4 battery packs, 1 Ambient Temperature & 1 Ambient Pressure 
CAN_message_t current4; //Packet2: Current from 4 battery packs
CAN_message_t temp4;  //Packet3: Temp from 4 battery packs

/*function definations*/
/*sensor functions*/
void voltageFunc(void * args);//arduino Mini serial input   //event bits : 1UL<<1UL
void voltageISR();
void gy_bmp_280(void *args);//temperature sensor
double adc_current(int8_t readAddress);//reading specified ADC channel
double adc_thermistor(int8_t readAddress);  //read ADC for thermistor channels
void adc_current_thermistor(void *args);  //read ADc for voltage channels
//void lv_battery_sensor(void *args);
/*actuators functions*/
void HBridge(void *args); //event bits : 1UL<<2UL
void clutch(void *args);  //event bits : 1UL<<3UL
void contactor(void *args); //event bits : 1UL<<4UL
void stepper(void *args); //event bits : 1UL<<5UL
/*node general functions*/
void functionTest(void *args);  //event bits : 1UL<<6UL
void CANSend(void *args); //send general data packets with sensor values
void CANReceive1(void *args);  //analyse data packet 1 from CAN
void CANReceive2(void *args);  //analyse SystemState packet from CAN
void CANReceive3(void *args);  //analyse data packet 3 from CAN
void CANReceive4(void *args);  //analyse data packet 4 from CAN
void CANReceive5(void *args);  //analyse data packet 5 from CAN
void CANReceive6(void *args);  //analyse data packet 6 from CAN
void CANReceive7(void *args);  //analyse data packet 7 from CAN


//Stepper stepper1(STEPPER_REV,STEPPER11,STEPPER12,STEPPER13,STEPPER14);

/*define clases to be used for DC motor driving*/
// ??
// class CytronMotor{
  
//   int state;

//   /*
//     0 - stop
//     1 - move foward
//   */

//   public:

//     CytronMotor(int br_s1=9600, int br_s2=9600){
//       Talk_Motor.begin(br_s1);
//       Serial1.begin(br_s2);
//       //cli(); //Disable interrupts
//       state = 0;
//       Talk_Motor.write(0);
//       Talk_Motor.write(128);
//       sei(); //Enable interrupts
//       Serial1.println("Low speed Drive motor initialised correctly");

//     }


//     void moveforward(int level){
//       /*
//         Right motor - 192 (11000000) to 255 (11111111)
//         Left motor - 64 (01000000) to 127 (01111111)

//       */

//       if(level>0 && level<=63){
//         //cli(); //Disable interrupts
//         state=1;
//         Talk_Motor.write(192+level);
//         Talk_Motor.write(64+level);
//         sei(); //Enable interrupts
//       }

//       else
//         Serial1.println("Value not permitted");
//     }

//     void stop(){
//       /*
//         Left motor stop - 0 or 64
//         Right motor stop - 128 or 192
//       */
//       //cli(); //Disable interrupts
//       state = 0;
//       Talk_Motor.write(0);
//       Talk_Motor.write(128);
//       sei(); //Enable interrupts
//     } 

//     int getState(){
//       return state;
//     }

// };


// ??

/*CAN listener class*/
class CanListener1 : public CANListener 
{
public:
  bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

CanListener1 canListener1;

bool CanListener1::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
  msgRec=frame;
  // switch(mailbox){
  //   case 0: xSemaphoreGiveFromISR(PKTHDLE1, pdFALSE);break;    //vTaskNotifyFromISR?
  //   case 1: xSemaphoreGiveFromISR(PKTHDLE2, pdFALSE);break;
  //   case 2: xSemaphoreGiveFromISR(PKTHDLE3, pdFALSE);break;
  //   case 3: xSemaphoreGiveFromISR(PKTHDLE4, pdFALSE);break;
  //   case 4: xSemaphoreGiveFromISR(PKTHDLE5, pdFALSE);break;
  //   case 5: xSemaphoreGiveFromISR(PKTHDLE6, pdFALSE);break;
  //   case 6: xSemaphoreGiveFromISR(PKTHDLE7, pdFALSE);break;
  // }
  return true;
}


void honey_well(void* args){
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


/*

    if((out & 0xC000 == 0x0000)){
      //Normal operation
      local_pressure =(out - 0x0666)*1.6/(0x3999 - 0x0666);
      xSemaphoreTake( accMutex,portMAX_DELAY);
      global_pressure = local_pressure;
      xSemaphoreGive( accMutex );
      Serial1.print("Pressure: ");
      Serial1.println(local_pressure);
    }

    else if((out & 0xC000) == 0x8000){
      // 10 Stale Data
      Serial1.println("Power: Pressure Sensor data stale");
    }

    else if((out & 0xC000) == 0xC000){
      //11 Diagnostic condition
      Serial1.println("Power: Pressure Sensor not working");
    }

    else if((out & 0xC000) == 0x4000){
      //01 Used for programming the sensor
      Serial1.println("Power: Pressure Sensor in programming mode");
    }

    else{
      Serial1.println("Power: Unexpected system state");
    }
*/
    vTaskDelay(pdMS_TO_TICKS(PRESSURE_DELAY));
  }

}


/*
*identify the battery voltage based on the input from arduino mini
*If the serial value < 100, battery 1 and 2 values are Received
*else battery 3 and 4 values are Received.
*the serial value is of the format: Serial_ID:Battery_n:battery_n+1
*/
// void voltageFunc(void * args){
//   while(1){
//     //xEventGroupWaitBits(grp,1UL<<1UL,pdTRUE,pdTRUE,portMAX_DELAY);
//     ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
//     if(Mini>=100){
//       MiniVoltage[2]=Mini%10;
//       Mini/=10;
//       MiniVoltage[3]=Mini%10;
//     }
//     else{
//       MiniVoltage[0]=Mini%10;
//       Mini/=10;
//       MiniVoltage[1]=Mini%10;
//     }
    
//     Serial1.print(" mini battery 1=");
//     Serial1.print(MiniVoltage[0]);
//     Serial1.print(" mini battery 2=");
//     Serial1.print(MiniVoltage[1]);
//     Serial1.print(" mini battery 3=");
//     Serial1.print(MiniVoltage[2]);
//     Serial1.print(" mini battery 4=");
//     Serial1.println(MiniVoltage[3]);      
//   }
// }

// //ISR raised when data is available on serial4 from arduino Mini
// void voltageISR(){
//   BaseType_t xHigherPriorityTaskWoken=pdFALSE;
//   if(Serial4.available()>0){
//     Mini=Serial4.read();    
//     //xEventGroupSetBitsFromISR(grp, 1UL << 1UL, &xHigherPriorityTaskWoken);
// //    vTaskNotifyGiveFromISR(voltFunc);
//   }
// }

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


/*ADC function to read values on the specified ADC channel*/
double adc_hall(int8_t readAddress){
  double current=0;
  int adc_reading = 0;
  byte dataMSB =0;
  byte dataLSB =0;
  byte JUNK = 0;

  SPI.beginTransaction (adc);
  digitalWrite         (CS, LOW);
  SPI.transfer         (0x01);                                 // Start Bit
  dataMSB =            SPI.transfer(readAddress << 4) & 0x03;  // Send readAddress and receive MSB data, masked to two bits
  dataLSB =            SPI.transfer(JUNK);                     // Push junk data and get LSB byte return
  digitalWrite         (CS, HIGH);
  SPI.endTransaction   ();     
  adc_reading = dataMSB << 8 | dataLSB;
  //current = (adc_reading-ADC_OFFSET_CURRENT-512)*75.8;
  return adc_reading;
}

double adc_thermistor(int8_t readAddress){
  int adc_reading = 0;
  byte dataMSB =0;
  byte dataLSB =0;
  byte JUNK = 0;

  SPI.beginTransaction (adc);
  digitalWrite         (CS, LOW);
  SPI.transfer         (0x01);                                 // Start Bit
  dataMSB =            SPI.transfer(readAddress << 4) & 0x03;  // Send readAddress and receive MSB data, masked to two bits
  dataLSB =            SPI.transfer(JUNK);                     // Push junk data and get LSB byte return
  digitalWrite         (CS, HIGH);
  SPI.endTransaction   ();
     
  adc_reading = dataMSB << 8 | dataLSB;
  return adc_reading;
}


/*get thermistor values from ADC
*values are inputted from CH0 to CH3
*these values are converted from 0-5 to 0-1024 range
*/

/*get hall sensors voltage values from ADC 
*values are inputted from CH4 to CH7
*these values are converted from 0-5 to 0-1024 range
*/
void adc_current_thermistor(void *args){

  SPI.begin();
  pinMode(CS,OUTPUT);
  digitalWrite(CS,LOW);
  digitalWrite(CS,HIGH);
  //SPI.beginTransaction(adc);
  //digitalWrite(CS,LOW);

  uint16_t ADCthermistor_local[4] = {0,0,0,0};    //ADC thermistor value
  uint16_t ADCcurrent_local[4] = {0,0,0,0}; 
  float temp_local[4] = {0,0,0,0};      //ADC current value
  float cur_local[4] = {0,0,0,0};

  for (;;){
    ADCthermistor_local[0]=adc_thermistor(CH0);
    ADCthermistor_local[1]=adc_thermistor(CH1);
    ADCthermistor_local[2]=adc_thermistor(CH2);
    ADCthermistor_local[3]=adc_thermistor(CH3);
    ADCcurrent_local[0]=adc_hall(CH4);
    ADCcurrent_local[1]=adc_hall(CH5);
    ADCcurrent_local[2]=adc_hall(CH6);
    ADCcurrent_local[3]=adc_hall(CH7);

    xSemaphoreTake( accMutex,portMAX_DELAY);
    
    ADCthermistor_global[0] = ADCthermistor_local[0];
    ADCthermistor_global[1] = ADCthermistor_local[1];
    ADCthermistor_global[2] = ADCthermistor_local[2];
    ADCthermistor_global[3] = ADCthermistor_local[3];
    ADCcurrent_global[0] = ADCcurrent_local[0];
    ADCcurrent_global[1] = ADCcurrent_local[1];
    ADCcurrent_global[2] = ADCcurrent_local[2];
    ADCcurrent_global[3] = ADCcurrent_local[3];
    
    xSemaphoreGive( accMutex );
    
   /* Serial1.print(" temperature 1=");
    Serial1.print(ADCthermistor_local[0]);
    Serial1.print(" temperature 2=");
    Serial1.print(ADCthermistor_local[1]);
    Serial1.print(" temperature 3=");
    Serial1.print(ADCthermistor_local[2]);
    Serial1.print(" temperature 4=");
    Serial1.println(ADCthermistor_local[3]);*/
            
    temp_local[0] = 25 - log10(1023.0/ADCthermistor_local[0] - 1)/BETA;
    temp_local[1] = 25 - log10(1023.0/ADCthermistor_local[1] - 1)/BETA;
    temp_local[2] = 25 - log10(1023.0/ADCthermistor_local[2] - 1)/BETA;
    temp_local[3] = 25 - log10(1023.0/ADCthermistor_local[3] - 1)/BETA;

    Serial1.print(" temperature 1=");
    Serial1.print(temp_local[0],6);
    Serial1.print(" temperature 2=");
    Serial1.print(temp_local[1],6);
    Serial1.print(" temperature 3=");
    Serial1.print(temp_local[2],6);
    Serial1.print(" temperature 4=");
    Serial1.println(temp_local[3],6);

    Serial1.print(" current 1=");
    Serial1.println(ADCcurrent_local[0]);
    Serial1.print(" current 2=");
    Serial1.print(ADCcurrent_local[1]);
    Serial1.print(" current 3=");
    Serial1.print(ADCcurrent_local[2]);
    Serial1.print(" current 4=");
    Serial1.println(ADCcurrent_local[3]);
    
    cur_local[0] = ((ADCcurrent_local[0]/1023.0)*5.0 - 2.5)/0.066;
    Serial1.println(cur_local[0]);


    //code for the LV battery voltage
    analogReadResolution(16);
    Serial1.print("Secondary battery Voltage = ");
    Serial1.println(analogRead(A22)/1023*3.3*9.33);
    // that ends here



    vTaskDelay(pdMS_TO_TICKS(ADC_CURRENT_TEMPERATURE_DELAY));
  }
}


//CAN dependent
//activate HBridge
// void HBridge(void *args){
//   while(1){
//     //xEventGroupWaitBits(grp,1UL<<2UL,pdTRUE,pdTRUE,portMAX_DELAY);
//     ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
//     digitalWrite(HBRIDGE_D,directionHBridge);
//     digitalWrite(HBRIDGE_E,enableHBridge);
//   }
// }

// //activate clutch
// void clutch(void *args){
//   while(1){
//     //xEventGroupWaitBits(grp,1UL<<3UL,pdTRUE,pdTRUE,portMAX_DELAY);
//     ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
//     digitalWrite(CLUTCH,activateClutch);
//     //Serial1.println(activateClutch);
//   }
// }

// //activate battery contactor
// void contactor(void *args){
//   while(1){
//     //xEventGroupWaitBits(grp,1UL<<4UL,pdTRUE,pdTRUE,portMAX_DELAY);
//     ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
//     digitalWrite(CONTACTOR,activateContactor);
//     //Serial1.println(activateContactor);
//   }
// }

// //rotate the stepper motor
// void stepper(void *args){
//   while(1){
//     //xEventGroupWaitBits(grp,1UL<<5UL,pdTRUE,pdTRUE,portMAX_DELAY);
//     ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
//     stepper1.step(STEPPER_REV);
//   }
// }

//HoneyWell, Stepper, Cytron Functions yet to be written

//send CAN data
void CANSend(void *args){

// volatile int Mini;//serial input from Mini
// volatile int MiniVoltage[4]={0,0,0,0};//indivitual voltages from mini
  uint8_t local_temperature = 0;//temperature sensor value          ??Change data type?
  float local_pressure = 0 ;  //Pressure Sensors   
  uint16_t ADCthermistor_local[4] = {0,0,0,0};     //ADC thermistor value
  uint16_t ADCcurrent_local[4] = {0,0,0,0};        //ADC voltage value
// volatile bool activateClutch=1;//clutch activation control variable
// volatile bool directionHBridge=0;//set direction for HBridge        ??How many variables for HBridge?
// volatile bool enableHBridge=0;//enable the HBridge: PWM here
// volatile bool activateContactor=0;//contactor activation control variable

//   *(volt4Temp1Press1.buf) = ;
//   volt4Temp1Press1.id = 0x14  //20
//   volt4Temp1Press1.len = 8;
//   current4.ext = 0;
//   current4.id = 0x15  //21;
//   current4.len = 8; 
//   temp4.ext = 0;
//   temp4.id = 0x16;  //22
//   temp4.len = 8;

  
//   msg.ext = 0;
//   msg.len = 8;
//   while(1){
    
//     msg.id = CAN_ID1;
//     msg.buf[0] = 0;
//     msg.buf[1] = 'n';
//     msg.buf[2] = 'o';
//     msg.buf[3] = 'd';
//     msg.buf[4] = 'e';
//     msg.buf[5] = ':';
//     msg.buf[6] = ' ';
//     msg.buf[7] = '1';
//     Can0.write(msg);

//     msg.id = CAN_ID2;
//     msg.buf[0] = 0;
//     msg.buf[1] = 'n';
//     msg.buf[2] = 'o';
//     msg.buf[3] = 'd';
//     msg.buf[4] = 'e';
//     msg.buf[5] = ':';
//     msg.buf[6] = ' ';
//     msg.buf[7] = '2';
//     Can0.write(msg);

//     msg.id = CAN_ID3;
//     msg.buf[0] = 0;
//     msg.buf[1] = 'n';
//     msg.buf[2] = 'o';
//     msg.buf[3] = 'd';
//     msg.buf[4] = 'e';
//     msg.buf[5] = ':';
//     msg.buf[6] = ' ';
//     msg.buf[7] = '3';
//     Can0.write(msg);
    
//     vTaskDelay(pdMS_TO_TICKS(CAN_DELAY));
//   }
  for(;;){
    vTaskDelay(pdMS_TO_TICKS(CAN_DELAY));
  }
}


/*void lv_battery_sensor(void *args)
{
  analogReadResolution(16);
  Serial1.println(analogRead(A22)/65535*3.3*9.34);
  vTaskDelay(pdMS_TO_TICKS(LV_DELAY));
}*/
// //CAN actuation for braking module
// void CANReceive1(void *args){
//   //ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
//   //xTaskNotifyWait(0, 0, &);
//   //frequency 3khz/constant to 0khz---> linearly decrease
//   //frequency/constant = rpm;
//   xSemaphoreTake(PKTHDLE1, portMAX_DELAY);
// }

// //CAN system state actuation
// void CANReceive2(void *args){
//   //uint32_t RcvBuffer;
//   xSemaphoreTake(PKTHDLE2, portMAX_DELAY);
//   int state_test = msgRec.buf[0];
//   //ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
//   //xTaskNotifyWait(0, 0, &RcvBuffer, portMAX_DELAY);
//   //state_tests = *((uint8_t*)(&RcvBuffer));
//   taskDISABLE_INTERRUPTS();
//   switch(state_test){
//     case 0: {         //Power On; Low Voltage Electronics powered on
//     //xTaskNotifyGive(PKTHDLE1);
//     system_state = 0;   // Use locks or mutex?
//     break;    
//     }
//     case 1: {         //Idle; from Power node (High Voltage stuff is on)
//     //report error 
//     break;    
//     }
//     case 2: {         //Ready; from Comm Node (signal that Health Check is Cool)
//       system_state = 2;
//       break;
//     }
//     case 3: {         //Awaiting Pusher Attachment; from Comm Node (signal from UI to launch Pod)
//       system_state = 3;
//       break;
//     }
//     case 4: {         //Pushing; From Nav node
//       system_state = 4;
//       break;
//     }
//     case 5: {         //Levitation & Breaking; From Nav node          
//       system_state = 5;
//       break;
//     }
//     case 6: {
//       system_state = 6;   //Descent & retraction of brakes; From Nav node
//       break;
//     }
//     case 7: {          //Rolling on Wheels?? LTS height trigger?     
//       //system_state = 7;
//       //report error;
//       break;
//     }  
//     case 8: {         //Low Speed Drive; from Nav node
//       system_state = 8;
//       break;
//     }
//     case 9: {         //Pod Stop; from Nav node
//       system_state = 9;
//       break;
//     }
//     case 10: {        //Emergency Stop; from Nav node
//       system_state = 10;
//       break;
//     }
//     case 11: {        //EB0 Should come from Nav on Sensor failure!!?
//       system_state = 11;
//       break;
//     }
//     case 12: {        //EB1 Clutch disengagement failed
//       //system_state = 12;
//       //report error
//       break;
//     }
//     case 13: {        //EB2 Contactor disengagement failed
//       system_state = 13;
//       break;
//     }
//     case 14: {        //Power Off
//       system_state = 14;
//       break;
//     }
//   }
//   taskENABLE_INTERRUPTS();
// }

// //CAN actuation for LSD actuation
// void CANReceive3(void *args){
//   //ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
//   //PWM frequency
//   //xTaskNotifyWait();
//   xSemaphoreTake(PKTHDLE3, portMAX_DELAY);
// }

// //CAN health packet Receive
// void CANReceive4(void *args){
//   //ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
//   //xTaskNotifyWait();
//   xSemaphoreTake(PKTHDLE4, portMAX_DELAY);
// }

// //CAN offline brake actuation
// void CANReceive5(void *args){
//   //ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
//   //xTaskNotifyWait();
//   xSemaphoreTake(PKTHDLE5, portMAX_DELAY);
// }

// //CAN offline LSD actuation
// void CANReceive6(void *args){
//   //ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
//   //xTaskNotifyWait();
//   xSemaphoreTake(PKTHDLE6, portMAX_DELAY);
// }

// //CAN offline clutch actuation
// void CANReceive7(void *args){
//   //ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
//   //xTaskNotifyWait();
//   xSemaphoreTake(PKTHDLE7, portMAX_DELAY);
// }

//setup the pinModes and create tasks

int main(){
    //Serial Setup
  Serial1.begin(Serial_BAUD);
//  Serial4.begin(Serial_BAUD);
//  Serial4.begin(Serial_BAUD);
  //CAN Setup
  Can0.begin(1000000);
  Can0.attachObj(&canListener1);
  canListener1.attachGeneralHandler();
  // canListener.attachMBHandler(0);
  // canListener.attachMBHandler(1);
  // canListener.attachMBHandler(2);
  // canListener.attachMBHandler(3);
  // canListener.attachMBHandler(4);
  // canListener.attachMBHandler(5);
  // canListener.attachMBHandler(6);

  // sys_state_change.ext = 0;
  // sys_state_change.id = 0x05; //5`
  // sys_state_change.len = 8;
  // sys_state_ack.ext = 0;
  // sys_state_ack.id = 0x09; //9
  // sys_state_ack.len = 8;
  // health_ack.ext = 0;
  // health_ack.id = 0x01; //1
  // health_ack.len = 1;
  // volt4Temp1Press1.ext = 0;
  // volt4Temp1Press1.id = 0x14;  //20
  // volt4Temp1Press1.len = 8;
  // current4.ext = 0;
  // current4.id = 0x15;  //21;
  // current4.len = 8; 
  // temp4.ext = 0;
  // temp4.id = 0x16;  //22
  // temp4.len = 8;  

  // CAN_filter_t filter;
  // // Framehandler invoked only for id 560
  // filter.flags.extended=0;
  // filter.flags.remote=0;
  // filter.flags.reserved=0;
  // filter.id=FILTER1;
  // Can0.setFilter(filter,0);
  // Can0.setMask(0x1FFFFFFF,0);

  // filter.id=FILTER2;
  // Can0.setFilter(filter,1);
  // Can0.setMask(0x1FFFFFFF,1);

  // filter.id=FILTER3;
  // Can0.setFilter(filter,2);
  // Can0.setMask(0x1FFFFFFF,2);

  // filter.id=FILTER4;
  // Can0.setFilter(filter,3);
  // Can0.setMask(0x1FFFFFFF,3);

  // filter.id=FILTER5;
  // Can0.setFilter(filter,4);
  // Can0.setMask(0x1FFFFFFF,4);

  // filter.id=FILTER6;
  // Can0.setFilter(filter,5);
  // Can0.setMask(0x1FFFFFFF,5);

  // filter.id=FILTER7;
  // Can0.setFilter(filter,6);
  // Can0.setMask(0x1FFFFFFF,6);

  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  delayMicroseconds(1000000);
  digitalWrite(13,LOW);

  pinMode(HBRIDGE_E,OUTPUT);
  pinMode(HBRIDGE_D,OUTPUT);
  pinMode(CLUTCH,OUTPUT);
  pinMode(CONTACTOR,OUTPUT);
  
  pinMode(A22,INPUT);

//  stepper1.setSpeed(STEPPER_SPEED);

//  attachInterrupt(VOLTAGEIN,voltageISR,CHANGE);
  grp=xEventGroupCreate();
  accMutex = xSemaphoreCreateMutex();
  // PKTHDLE1 = xSemaphoreCreateMutex();   //Mutexes to redirect control from FrameHandler to CANRcv tasks 
  // PKTHDLE2 = xSemaphoreCreateMutex();
  // PKTHDLE3 = xSemaphoreCreateMutex();
  // PKTHDLE4 = xSemaphoreCreateMutex();
  // PKTHDLE5 = xSemaphoreCreateMutex();
  // PKTHDLE6 = xSemaphoreCreateMutex();
  // PKTHDLE7 = xSemaphoreCreateMutex();

//  xTaskCreate(voltageFunc, "funcVoltage", configMINIMAL_STACK_SIZE, NULL, 4, &voltFunc);
  xTaskCreate(gy_bmp_280, "gy_bmp_280", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
  xTaskCreate(honey_well, "honey_well", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
  xTaskCreate(adc_current_thermistor, "adc_current_thermistor", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
  // xTaskCreate(HBridge, "HBridge", configMINIMAL_STACK_SIZE, NULL, 10, NULL);
  // xTaskCreate(clutch, "clutch", configMINIMAL_STACK_SIZE, NULL, 10, NULL);
  // xTaskCreate(contactor, "contactor", configMINIMAL_STACK_SIZE, NULL, 10, NULL);
  xTaskCreate(CANSend, "CANSend", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
  //xTaskCreate(lv_battery_sensor, "lv_battery_sensor",configMINIMAL_STACK_SIZE,NULL,4,NULL);
  // xTaskCreate(CANReceive1, "CANReceive1", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(CANReceive2, "CANReceive2", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(CANReceive3, "CANReceive3", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(CANReceive4, "CANReceive4", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(CANReceive5, "CANReceive5", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(CANReceive6, "CANReceive6", configMINIMAL_STACK_SIZE, NULL, 8, NULL);
  // xTaskCreate(CANReceive7, "CANReceive7", configMINIMAL_STACK_SIZE, NULL, 8, NULL);

  vTaskStartScheduler();
  for(;;);  
  return 0;
}


