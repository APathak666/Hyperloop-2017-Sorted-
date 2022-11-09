#include <Arduino.h>
int dat=0;
void serialIn();
void setup() {
  Serial1.begin(9600);
  Serial4.begin(9600);
  attachInterrupt(31,serialIn,CHANGE);
  
  
}
void serialIn() {
    Serial1.println("in func");
    if(Serial4.available()>0){
      Serial1.println("in if");
      dat=Serial4.read();
      Serial1.println(dat);
    }
}

int main(){
  setup();
  for(;;){
    Serial1.println("vacant0");
    delayMicroseconds(500000);
    Serial1.println("vacant1");
    delayMicroseconds(500000);
  }
  return 0;

}