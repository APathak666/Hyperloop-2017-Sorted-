
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#define BNO055_SAMPLERATE_DELAY_MS (10)
#define multiplier 57.295779513082320876798154814105;

Adafruit_BNO055 bno = Adafruit_BNO055(55,0x28);

Adafruit_BNO055 bno2 = Adafruit_BNO055(55,0x29);

unsigned long old_time = 0;
unsigned long new_time = 0;

double ax=0,ay=0,az=0;
double ax0=0,ay0=0,az0=0;
double yaw=0,pitch=0,roll=0;
double vx=0,vy=0,vz=0;
double x=0,y=0,z=0,x0=0;
double x_2=0,y_2=0,z_2=0,x0_2=0;
int32_t dt = 10;



double pos_moving_average[10];

void rotate_array(double arr[], double temp);
double sum_arr(double arr[]);
double window_acc(double val);

imu::Vector<3> acc;
imu::Vector<3> acc2;
imu::Vector<3> eul;
imu::Vector<3> eul2;
char print_str[80];



#define DBYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define DBYTE_TO_BINARY(byte)  \
  (byte & 0x80000000 ? '1' : '0'), \
  (byte & 0x40000000 ? '1' : '0'), \
  (byte & 0x20000000 ? '1' : '0'), \
  (byte & 0x10000000 ? '1' : '0'), \
  (byte & 0x08000000 ? '1' : '0'), \
  (byte & 0x04000000 ? '1' : '0'), \
  (byte & 0x02000000 ? '1' : '0'), \
  (byte & 0x01000000 ? '1' : '0'), \
  (byte & 0x00800000 ? '1' : '0'), \
  (byte & 0x00400000 ? '1' : '0'), \
  (byte & 0x00200000 ? '1' : '0'), \
  (byte & 0x00100000 ? '1' : '0'), \
  (byte & 0x00080000 ? '1' : '0'), \
  (byte & 0x00040000 ? '1' : '0'), \
  (byte & 0x00020000 ? '1' : '0'), \
  (byte & 0x00010000 ? '1' : '0'), \ 
  (byte & 0x00008000 ? '1' : '0'), \
  (byte & 0x00004000 ? '1' : '0'), \
  (byte & 0x00002000 ? '1' : '0'), \
  (byte & 0x00001000 ? '1' : '0'), \
  (byte & 0x00000800 ? '1' : '0'), \
  (byte & 0x00000400 ? '1' : '0'), \
  (byte & 0x00000200 ? '1' : '0'), \
  (byte & 0x00000100 ? '1' : '0'), \
  (byte & 0x00000080 ? '1' : '0'), \
  (byte & 0x00000040 ? '1' : '0'), \
  (byte & 0x00000020 ? '1' : '0'), \
  (byte & 0x00000010 ? '1' : '0'), \
  (byte & 0x00000008 ? '1' : '0'), \
  (byte & 0x00000004 ? '1' : '0'), \
  (byte & 0x00000002 ? '1' : '0'), \
  (byte & 0x00000001 ? '1' : '0') 

void setup()
{

  Serial.begin(9600);
  if(!bno.begin() || !bno2.begin()){
    Serial.println("IMU not detected");
    while(1);
  }

for (int i=0;i<100;i++){
  acc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  acc2= bno2.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
   int16_t ax2[3]={acc.x(),acc.y(),acc.z()};
   int16_t ax4[3]={acc2.x(),acc2.y(),acc2.z()};
   //Serial.print(ax2[0]);
   //ax0+=ax2[0];
   x0 += ax2[0]*dt*dt*9.81/1000000;
   x0_2+=ax4[0]*dt*dt*9.81/1000000;
   //Serial.print("\t");
   Serial.print(x0);
   Serial.print("\t");
   Serial.println(x0_2);
   delay(100);
    
}
//ax0=ax0/100;
x0=x0/100;
x0_2=x0_2/100;
Serial.print("This is how much it increases by\t");
Serial.println(x0);
Serial.print("This is how much it increases by\t");
Serial.println(x0_2);

/*
for(int i=0;i<10;i++)
{
  acc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  int16_t ax2[3]={acc.x(),acc.y(),acc.z()};
  x+=(ax2[0]*dt*dt*9.81/1000000 - x0);
  pos_moving_average[i]=x;
  delay(1);
}
*/



  bno.setExtCrystalUse(true);
  bno2.setExtCrystalUse(true);


}
int32_t aaxx =0,posx=0,x_2_test=0,aazz=0,aayy=0;

void loop()
{
  new_time=micros();
  //dt = new_time - old_time;
  
  
  acc = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  //eul = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  acc2 = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  //eul2 = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  

  
  int32_t ax1[3]={acc.x(),acc.y(),acc.z()};
  int32_t ax3[3]={acc2.x(),acc2.y(),acc2.z()};
  aaxx|=abs(ax3[0]);
  sprintf(print_str,DBYTE_TO_BINARY_PATTERN,DBYTE_TO_BINARY(aaxx));
  Serial.print(print_str);
  Serial.print("\t");
  aazz|=abs(ax3[2]);
  sprintf(print_str,DBYTE_TO_BINARY_PATTERN,DBYTE_TO_BINARY(aazz));
  Serial.println(print_str);
  aayy|=abs(ax3[1]);
  sprintf(print_str,DBYTE_TO_BINARY_PATTERN,DBYTE_TO_BINARY(aayy));
  Serial.print(print_str);
  Serial.print("\t");
  
  
  
  //Serial.println(ax1[0]);
  //Serial.print("\t\t\t");
  /*Serial.print(ax1[1]);
  Serial.print("\t");
  Serial.print(ax1[2]);
  Serial.print("\t\n");
*/

  
 /* x += (ax1[0]*dt*dt*9.81/1000000 -x0);
  //x_2+=(ax3[0]*dt*dt*9.81/1000000 -x0_2);
  x_2_test += ax3[0]*dt*dt;
  Serial.print(abs(x_2_test));
  Serial.print("\t");*/


  /*posx |=abs(x_2_test);
  sprintf(print_str,DBYTE_TO_BINARY_PATTERN,DBYTE_TO_BINARY(posx));
  Serial.println(print_str);*/
  //Serial.print(x);
  //Serial.print("\t");
  //Serial.println(x_2);
  //rotate_array(pos_moving_average,x);
  //Serial.println(sum_arr(pos_moving_average)/10);
  //Serial.println(ax1[0]);
  //Serial.print("\t");
  //Serial.println(ax1[0]);
  
  //Serial.println(ax1[0],BIN);  

  
  //displayCalStatus();*/
  //imu::Quaternion quat = bno.getQuat();
  /* Display the quat data */
  /*Serial.print("qW: ");
  Serial.print(quat.w(), 4);
  Serial.print(" qX: ");
  Serial.print(quat.y(), 4);
  Serial.print(" qY: ");
  Serial.print(quat.x(), 4);
  Serial.print(" qZ: ");
  Serial.print(quat.z(), 4);
  Serial.println("");
  */
  delay(10);
  old_time = new_time;

}

void displayCalStatus(void)
{
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
 
  /* The data should be ignored until the system calibration is > 0 */
  Serial.print("\t");
  if (!system)
  {
    Serial.print("! ");
  }
 
  /* Display the individual values */
  Serial.print("Sys:");
  Serial.print(system, DEC);
  Serial.print(" G:");
  Serial.print(gyro, DEC);
  Serial.print(" A:");
  Serial.print(accel, DEC);
  Serial.print(" M:");
  Serial.println(mag, DEC);
}



void rotate_array(double arr[], double temp)
{
  int i;
  for(i=0;i<9;i++)
  {
    arr[i]=arr[i+1];
  }
  arr[i]=temp;
}






double sum_arr(double arr[])
{
  double sum=0;
  for(int i =0;i<10;i++)
  {
    sum+=arr[i];
  }
  return sum;
}



double window_acc(double val)
{

  if(val<=4 && val>=-4)
    return 0;
  else
    return val; 

}
