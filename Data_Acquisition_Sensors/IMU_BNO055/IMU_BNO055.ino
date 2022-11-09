#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>


/* This driver reads raw data from the BNO055

   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3V DC
   Connect GROUND to common ground

   History
   =======
   2015/MAR/03  - First release (KTOWN)
*/

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno1 = Adafruit_BNO055(55,0x28);
//Adafruit_BNO055 bno2 = Adafruit_BNO055(55,0x29);

unsigned long old_time = 0;
unsigned long new_time = 0;

double acceleration1[3];
//unsigned long acceleration2[3];

double euler1[3];
//unsigned long euler2[3];

double velocity1[3]={0,0,0};
double displacement1[3]={0,0,0};

//unsigned long velocity2[3];
//unsigned long displacement2[3];

  //imu::Vector<3> acc1;
  //imu::Vector<3> acc2;
  //imu::Vector<3> eul1;
  //imu::Vector<3> eul2;
/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)
{

  Serial.begin(9600);
  Serial.println("Orientation Sensor Raw Data Test"); Serial.println("");

  /* Initialise the sensor */
  if(!bno1.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
  while(1);
  }

  /*if(!bno2.begin())
  {
    
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }*/

  delay(1000);


  int8_t temp = bno1.getTemp();
  Serial.print("Current Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.println("");
  
  bno1.setExtCrystalUse(true);
  //bno2.setExtCrystalUse(true);
  
  Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void)
{
  new_time=micros();
  
  // Possible vector values can be:
  // - VECTOR_ACCELEROMETER - m/s^2
  // - VECTOR_MAGNETOMETER  - uT
  // - VECTOR_GYROSCOPE     - rad/s
  // - VECTOR_EULER         - degrees
  // - VECTOR_LINEARACCEL   - m/s^2
  // - VECTOR_GRAVITY       - m/s^2
  
 imu::Vector<3> acc1 = bno1.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  //acc2 = bno2.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  imu::Vector<3> eul1 = bno1.getVector(Adafruit_BNO055::VECTOR_EULER);
  //eul2 = bno2.getVector(Adafruit_BNO055::VECTOR_EULER);  

    imu::Vector<3> gra1 = bno1.getVector(Adafruit_BNO055::VECTOR_GRAVITY);

 
  
  /* Display the floating point data */
 /* Serial.print("X: ");
  Serial.print(acc1.x());
  Serial.print(" Y: ");
  Serial.print(acc1.y());
  Serial.print(" Z: ");
  Serial.print(acc1.z());
  Serial.print("\t\t");*/

  euler1[0]=eul1.x()*3.141592/180.0;
  euler1[1]=eul1.y()*3.141592/180.0;
  euler1[2]=eul1.z()*3.141592/180.0;
  //euler2[0]=eul2.x()*PI/180;
  //euler2[1]=eul2.y()*PI/180;
  //euler2[2]=eul2.z()*PI/180;

  acceleration1[0]=(acc1.x()-gra1.x())*cos(euler1[2])*cos(euler1[1])+(acc1.y()-gra1.y())*sin(euler1[2])*sin(euler1[1])-(acc1.z()-gra1.z())*sin(euler1[1]);
  //acceleration2[0]=acc2.x()*cos(euler2[2])*cos(euler2[1])+acc2.y()*sin(euler2[2])*sin(euler2[1])-acc2.z()*sin(euler2[1]);
  
  //displacement1[0]=velocity1[0]*(new_time-old_time)+0.5*acceleration1[0]*(new_time-old_time)*(new_time-old_time);
  //displacement[1]=velocity[1]*(new_time-old_time)+0.5*acc1.y()*(new_time-old_time)*(new_time-old_time);
  //displacement[2]=velocity[2]*(new_time-old_time)+0.5*acc1.z()*(new_time-old_time)*(new_time-old_time);

  //displacement2[0]=velocity2[0]*(new_time-old_time)+0.5*acceleration2[0]*(new_time-old_time)*(new_time-old_time);

  velocity1[0]+=acceleration1[0]*(new_time-old_time)/1000000;
  displacement1[0]+=velocity1[0]*(new_time-old_time)/1000000;
  //velocity2[0]=velocity2[0]+acceleration2[0]*(new_time-old_time);
  //velocity[1]=velocity[1]+acc1.y()*(new_time-old_time);
  //velocity[2]=velocity[2]+acc1.z()*(new_time-old_time);
  
 /*Serial.print("Euler X: ");
  Serial.print(euler1[0]);
  Serial.print("Euler  Y: ");
  Serial.print(euler1[1]);
  Serial.print(" Euler Z: ");
  Serial.print(euler1[2]);
  Serial.print("\t\t");*/
 
 
 
 /*Serial.print("Euler X: ");
  Serial.print(eul1.x());
  Serial.print("Euler  Y: ");
  Serial.print(eul1.y());
  Serial.print(" Euler Z: ");
  Serial.print(eul1.z());
  Serial.print("\t\t");
*/
    Serial.print("Acc X: ");
  Serial.print(acc1.x());
  Serial.print(" acc Y: ");
  Serial.print(acc1.y());
  Serial.print(" acc Z: ");
  Serial.print(acc1.z());
  Serial.print("\t\t");

    Serial.print("Gra X: ");
  Serial.print(gra1.x());
  Serial.print(" gra Y: ");
  Serial.print(gra1.y());
  Serial.print(" gra Z: ");
  Serial.print(gra1.z());
  Serial.print("\t\t");

  Serial.print("  Acc: ");
  Serial.print(acceleration1[0]);
  Serial.print("   vel: ");
  Serial.print(velocity1[0]);
  Serial.print("     displacement: ");
  Serial.print(displacement1[0]);
  Serial.print("\t");

  Serial.println(new_time);
  Serial.println("\t");
  Serial.println(old_time);
  Serial.println("\t");
  Serial.println(new_time-old_time);

Serial.println("\n");


  /*
  // Quaternion data
  imu::Quaternion quat = bno.getQuat();
  Serial.print("qW: ");
  Serial.print(quat.w(), 4);
  Serial.print(" qX: ");
  Serial.print(quat.y(), 4);
  Serial.print(" qY: ");
  Serial.print(quat.x(), 4);
  Serial.print(" qZ: ");
  Serial.print(quat.z(), 4);
  Serial.print("\t\t");
  */

  /* Display calibration status for each sensor. */
 /* uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  Serial.print("CALIBRATION: Sys=");
  Serial.print(system, DEC);
  Serial.print(" Gyro=");
  Serial.print(gyro, DEC);
  Serial.print(" Accel=");
  Serial.print(accel, DEC);
  Serial.print(" Mag=");
  Serial.println(mag, DEC);*/

  old_time=new_time;

  delay(BNO055_SAMPLERATE_DELAY_MS);
}
