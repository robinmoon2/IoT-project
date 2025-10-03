#include<screen.hpp>

void setup() {
  pinMode(Vext,OUTPUT);
    digitalWrite(Vext, LOW);
  // put your setup code here, to run once:
  Serial.begin(115200);
  InitScreen();
}

void loop() {
  display.drawString(10,10,"Hello World");
  display.display();
}
