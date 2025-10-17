#include <BME.hpp>
#include <heltec_unofficial.h>
#include <TMG3993.hpp>


// put fuCnction declarations here:
char buf[30];


void setup() {
  Serial.begin(115200);
  heltec_setup();

  display.setFont(ArialMT_Plain_10);
  display.drawString(0,0,"Hello, world!");
  while(!Serial);
  configurationTMG3993();
  configurationBME();
  display.display();
}

void loop() {
  getDataTMG3993();
  Serial.println();
  Serial.print("Looping...");
  getDataBME();
  display.drawString(0,0,"Hello, world!");
  delay(2000);

  snprintf(buf, sizeof(buf), "T: %.2f C", bme.temperature);
  display.drawString(10,10,buf);
  snprintf(buf, sizeof(buf), "P: %.2f hPa", bme.pressure/100.0);
  display.drawString(10,30,buf);
  snprintf(buf, sizeof(buf), "H: %.2f %%", bme.humidity);
  display.drawString(10,50,buf);
  snprintf(buf,sizeof(buf),"lux: %.2f ",tmg3993.getLux());
  display.drawString(10,70,buf);
  display.display();
}
