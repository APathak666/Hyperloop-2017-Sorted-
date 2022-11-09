#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>



int gy_bmp_280()
{
  Adafruit_BMP280 bme;
  Serial.begin(9600);
  //while(!Serial.available());
  if (!bme.begin())
  {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  for(;;)
  {
    Serial.print("---- GY BMP 280 ----------------\n");
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100); // 100 Pa = 1 millibar
    Serial.println(" mb");
    Serial.print("Approx altitude = ");
    Serial.print(bme.readAltitude(1013.25));
    Serial.println(" m");
    Serial.print("--------------------------------\n\n");
    delayMicroseconds(250000);
  }
  return 0;
}