#include <BME.hpp>
#include <heltec_unofficial.h>
#include <TMG3993.hpp>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include "LittleFS.h"
#include <ArduinoJson.h>
#include "Arduino.h"
#include <String.h>

#define ACTIVATION_PIN 35

struct DataStruct{
  float temperature;
  float pressure;
  float humidity;
  float light_intensity;
};

#include "LoRa.hpp"

const bool MAINBOARD = true;

String receivedataweb ="off";
const char* ssid = "A54cluzet";
const char* password = "alexandre2004";

WebServer server(80);
DataStruct data = {0.0f,0.0f,0.0f,0.0f};


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

void handleReceiveData(){
  HTTPClient http;
  http.begin("http://example.com/api/receive");
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
      String payload = http.getString();
      receivedataweb = payload;
      Serial.println("Data received from web: " + receivedataweb);
      display.clear();
      display.drawString(0,0,"Data from web:");
      display.drawString(0,10,receivedataweb);
      display.display();
       delay(1000);
  } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);
  }
  http.end();

}


void setup() {
  Serial.begin(115200);
  heltec_setup();
  // both the devices use LoRa communication method
  configurationLoRa();  

  if(MAINBOARD){
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

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
  server.on("/api/receive", handleReceiveData);

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
  }

  else{
    display.setFont(ArialMT_Plain_10);
    configurationBME();
    display.drawString(0,0,"Hello, world!");
    configurationTMG3993();
    
    print_wakeup_reason();
    if (heltec_wakeup_was_timer()) {
    heltec_deep_sleep(2000);
    }
    esp_sleep_enable_ext0_wakeup(WAKEUP_GPIO, 1);
    while(!Serial);
    pinMode(ACTIVATION_PIN,INPUT);
  }

  display.display();

}

void loop() {
  if(MAINBOARD){
    SendLoRa(1);
    data = ReceiveLoRa();
    server.handleClient();
    delay(10000);
  }
  else{
    getDataBME();
    getDataTMG3993();
    data.temperature = 20;
    data.pressure = 15;
    data.humidity = 74.3f;
    uint16_t r, g, b, c;
    tmg3993.getRGBCRaw(&r,&g,&b,&c);
    data.light_intensity = tmg3993.getLux(r,g,b,c);
    SendLoRa(data);
    delay(1000);
    esp_deep_sleep_start();
    
  }
  
}
  /*
  server.handleClient();
  display.clear();
  digitalWrite(LED_BUILTIN, LOW);
  getDataTMG3993();
  getDataBME();

  data1.pressure = bme.pressure;
  data1.temperature = bme.temperature;
  data1.light_intensity = tmg3993.getLux();
  data1.humidity = bme.humidity;

  snprintf(buf, sizeof(buf), "T: %.2f C", bme.temperature);
  display.drawString(10,10,buf);
  snprintf(buf, sizeof(buf), "P: %.2f hPa", bme.pressure/100.0);
  display.drawString(10,30,buf);
  snprintf(buf, sizeof(buf), "H: %.2f %%", bme.humidity);
  display.drawString(10,50,buf);
  snprintf(buf,sizeof(buf),"lux: %.2f ",tmg3993.getLux());
  display.drawString(10,70,buf);
  display.display();
}*/