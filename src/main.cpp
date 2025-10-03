#include <BME.hpp>
// put function declarations here:


void setup() {
  Serial.begin(115200);
  while(!Serial);
  configurationBME();
}

void loop() {
  getDataBME();
  delay(2000);
}
