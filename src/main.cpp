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
const bool MAINBOARD = false;

bool buttonWake = false;

String receivedataweb ="off";

const char* ssid = "A54cluzet";
const char* password = "alexandre2004";

WebServer server(80);
DataStruct data = {0.0f,0.0f,0.0f,0.0f};

void handleApiData() {
    DynamicJsonDocument doc(4096);
    doc["temperature"] = data.temperature;
    doc["humidity"] = data.humidity;
    doc["pressure"] = data.pressure/100;
    doc["lum"] = data.light_intensity;

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
      buttonWake = true;
       delay(1000);
  } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);
  }
  http.end();

}


void setup() {
  heltec_setup();

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

    configurationLoRa();    // both the devices use LoRa communication method

  // Start the server
  server.begin();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0,0,"Hello, world!");
  while(!Serial);
  }

  else{
    heltec_setup();

    configurationBME();
    configurationLoRa();    // both the devices use LoRa communication method

    display.setFont(ArialMT_Plain_10);
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
    if(buttonWake){
      SendLoRa(1);
      data = ReceiveLoRa();
      buttonWake = false;
      Serial.print("Temperature = ");
      Serial.print(data.temperature);
      Serial.println(" *C");

    Serial.print("Humidity =");
    Serial.print(data.humidity);
    Serial.println(" d");
    }
    server.handleClient();
  }
  
  else {
      Serial.println("LOOP");
      //vspi->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
      Serial.println("VSPI initialisation");
      //digitalWrite(BME_CS, LOW);
      Serial.println("DIGITAL WRITE LOW");
      getDataBME();
      Serial.println("DATA ACQUIRED");
      //digitalWrite(BME_CS, HIGH);
      vspi->endTransaction();

      getDataTMG3993();
      data.temperature = bme.temperature;
      data.pressure = bme.pressure;
      data.humidity = bme.humidity;

      Serial.println(data.temperature);
      uint16_t r, g, b, c;
      tmg3993.getRGBCRaw(&r,&g,&b,&c);
      data.light_intensity = tmg3993.getLux(r,g,b,c);

      SendLoRa(data);
      delay(1000);
      esp_deep_sleep_start();
  }

}
