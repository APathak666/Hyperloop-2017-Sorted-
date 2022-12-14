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
#include <event_groups.h>

#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "i2c_t3.h"
#endif

#define Serial Serial1

MPU6050 mpu;

#define OUTPUT_READABLE_WORLDACCEL

#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)

bool blinkState = false;
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
EventGroupHandle_t grp;//eventGroup handles for ISR and their related functions

// orientation/motion vars

Quaternion q;           // [w, x, y, z]         quaternion container

VectorInt16 aa;         // [x, y, z]            accel sensor measurements

VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements

VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements

VectorFloat gravity;    // [x, y, z]            gravity vector

float euler[3];         // [psi, theta, phi]    Euler angle container

float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

long counter = 0;

double acc_x, acc_y, acc_z;
double  v_x = 0, d_x=0;

// packet structure for InvenSense teapot demo

uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };

// ================================================================

// ===               INTERRUPT DETECTION ROUTINE                ===

// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high

//int delay1 = 500 / portTICK_PERIOD_MS;

void dmpDataReady() {

    mpuInterrupt = true;

}

// ================================================================

// ===                      INITIAL SETUP                       ===

// ================================================================

void setup() {

    // join I2C bus (I2Cdev library doesn't do this automatically)

    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE

        Wire.begin();

        //TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)

    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE

        Fastwire::setup(400, true);

    #endif

    Serial.begin(115200);

    while (!Serial); 

    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();
    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
    // wait for ready
    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    Serial.println(F("Fuck DMP"));
    devStatus = mpu.dmpInitialize();
    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(177);
    mpu.setYGyroOffset(-33);
    mpu.setZGyroOffset(-52);
    mpu.setZAccelOffset(870);
    mpu.setYAccelOffset(-1525);
    mpu.setXAccelOffset(-108); // 1688 factory default for my test chip
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);
        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        pinMode(5, INPUT_PULLUP);
        attachInterrupt(5, dmpDataReady, CHANGE);
        mpuIntStatus = mpu.getIntStatus();
        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;
        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
}

void accelerometer(void* args) {

    setup(); 

    for(;;) {

    xEventGroupWaitBits(grp,1UL<<1UL,pdTRUE,pdTRUE,portMAX_DELAY);

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10)) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
        Serial.print(fifoCount);

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        #ifdef OUTPUT_READABLE_WORLDACCEL
            // display initial world-frame acceleration, adjusted to remove gravity
            // and rotated based on known orientation from quaternion
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
            Serial.print("areal\t");
            acc_x = (aaWorld.x)*9.8/16384;
            Serial.print(acc_x);
            Serial.print("\t");
            acc_y = (aaWorld.y)*9.8/16384;
            Serial.print(acc_y);
            Serial.print("\t");
            acc_z = (aaWorld.z)*9.8/16384;
            Serial.print(acc_z);
            Serial.print("\t");
            Serial.print(counter);
            Serial.print("\t");
            if(counter>=2000){
                v_x = v_x + acc_x*0.005;
                d_x = d_x + v_x*0.005;
                Serial.print(v_x);
                Serial.print("\t");
                Serial.print(d_x); 
                Serial.print("\t");
            }
            Serial.println(".");
            counter++;
        #endif
    }}
}

int main()
{
    xTaskCreate(accelerometer, "ACC", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    vTaskStartScheduler();
    for(;;);
    return 0;
}
