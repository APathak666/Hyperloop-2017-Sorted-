#define l1 2
#define l2 3
#define l3 4
#define l4 5
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(l1,INPUT);
pinMode(l2,INPUT);
pinMode(l3,INPUT);
pinMode(l4,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.print(digitalRead(l1));
Serial.print(digitalRead(l2));
Serial.print(digitalRead(l3));
Serial.print(digitalRead(l4));
Serial.println();
}
