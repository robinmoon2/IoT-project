#include <BME.hpp>
#include <TMG3993.hpp>
// put function declarations here:


void setup() {
  Wire.begin();
  Serial.begin(115200);
  while(!Serial);
  configurationTMG3993();
  configurationBME();
}

void loop() {
  getDataTMG3993();
  Serial.println();
  getDataBME();
  delay(2000);

}
