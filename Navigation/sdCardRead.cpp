#include <kinetis.h>
#include <stdlib.h>
#include <Arduino.h>
#include <SD.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const int chipSelect = BUILTIN_SDCARD;
File dataFile;

int main(){
  Serial1.begin(9600);
  Serial1.println("Starting Main");
  SD.begin(chipSelect);    
  dataFile=SD.open("datalog.txt");
  while(dataFile){
    Serial1.print((char)dataFile.read());
  }
  return 0;
}