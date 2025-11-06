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

const char* ssid = "TelRobin";
const char* password = "robinestbeau";


WebServer server(80);

void handleApiData() {
    DynamicJsonDocument doc(4096);
    // Données intérieures
    doc["temperature"] = bme.temperature;
    doc["humidity"] = bme.humidity;
    doc["pressure"] = bme.pressure;
    // Données extérieures
    doc["temp_today"] = 0;
    doc["humidity_today"] = 0;
    doc["weather_today"] = 0;
    doc["id_weather_today"] = 0;
    doc["lastupdate"] = 0;
    doc["ip"] = WiFi.localIP().toString();
    // Pluie à venir
    JsonArray rain = doc.createNestedArray("rainfallhour");
    for (int i = 0; i < 12; i++) rain.add(0);
    // Prévisions 6 jours
    JsonArray forecast = doc.createNestedArray("forecast");
    for (int i = 0; i < 6; i++) {
        JsonObject day = forecast.createNestedObject();
        day["day"] = ""; // tu peux ajouter le nom du jour si tu veux
        day["temp_min"] = 0;
        day["temp_max"] = 0;
        day["humidity"] = 0;
        day["id_weather"] = 0;
    }
    String json;
    serializeJson(doc, json);
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
  heltec_setup();

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
