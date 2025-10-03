#include <BME.hpp>
#include <TMG3993.hpp>
#include<screen.hpp>
// put function declarations here:


void setup() {
  Wire.begin();
  Serial.begin(115200);
  
  pinMode(Vext,OUTPUT);
   digitalWrite(Vext, LOW);
  // put your setup code here, to run once:
  
  InitScreen();
  while(!Serial);
  configurationTMG3993();
  configurationBME();
}

void loop() {
  getDataTMG3993();
  Serial.println();
  getDataBME();
  
  delay(2000);
  
  display.drawString(10,10,"Hello World");
  display.display();
}
