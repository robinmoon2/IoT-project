#include <BME.hpp>
#include <heltec_unofficial.h>
#include <TMG3993.hpp>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include "LittleFS.h"
#include <ArduinoJson.h>
#include <String.h>

char buf[30];

const char* ssid = "A54cluzet";
const char* password = "alexandre2004";


WebServer server(80);

void handleApiData() {
    DynamicJsonDocument doc(4096);
    doc["temperature"] = bme.temperature;
    doc["humidity"] = bme.humidity;
    doc["pressure"] = bme.pressure/100;
    doc["lum"] = tmg3993.getLux();
    String json;
    serializeJson(doc, json);
    Serial.println("API data sent");
    server.send(200, "application/json", json);

}

void handleIndex() {
    File file = LittleFS.open("/index.html","r");
    if (!file) {
        server.send(404, "text/plain", "Fichier index.html non trouvé");
        return;
    }
    server.streamFile(file, "text/html");
    file.close();
}


void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (!LittleFS.begin()) {
    Serial.println("Erreur LittleFS");
    while (1);
  }

  server.on("/", handleIndex);
  server.on("/api/data", handleApiData);

  // Start the server
  server.begin();

  heltec_setup();

  display.setFont(ArialMT_Plain_10);
  display.drawString(0,0,"Hello, world!");
  while(!Serial);
  configurationTMG3993();
  configurationBME();
  display.display();
}

void loop() {
  server.handleClient();
  display.clear();

  getDataTMG3993();
  Serial.println();
  getDataBME();
  display.drawString(0,0,"Hello, world!");

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
