#include "main.h"

#define HELTEC_POWER_BUTTON
#include <heltec_unofficial.h>

void handleApiData() {
    DynamicJsonDocument doc(4096);
    doc["temperature"] = data.temperature;
    doc["humidity"] = data.humidity;
    doc["pressure"] = data.pressure/100;
    doc["lum"] = data.light_intensity;
    doc["water_level"] = data.water_level;

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
    heltec_deep_sleep(60);
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
      display.clear();
      SendLoRa(1);
      data = ReceiveLoRa();
      buttonWake = false;
      Serial.print("Temperature = ");
      Serial.print(data.temperature);
      Serial.println(" *C");

    Serial.print("Humidity =");
    Serial.print(data.humidity);
    Serial.println(" d");
    Serial.print("Water level = ");
    Serial.println(data.water_level);
    display.clear();
    display.drawString(0,0,"Temp: " + String(data.temperature) + " C");
    display.drawString(0,10,"Hum: " + String(data.humidity) + " %");
    display.drawString(0,20,"Pres: " + String(data.pressure/100) + " hPa");
    display.drawString(0,30,"Lum: " + String(data.light_intensity) + " lx");
    display.drawString(0,40,"Water lvl: " + String(data.water_level) + " %");

    display.display();

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
      data.light_intensity = tmg3993.getLux();
      data.water_level = analogRead(19);

      Serial.println(data.temperature);
      uint16_t r, g, b, c;
      tmg3993.getRGBCRaw(&r,&g,&b,&c);
      data.light_intensity = tmg3993.getLux(r,g,b,c);

      SendLoRa(data);
      delay(1000);
      esp_deep_sleep_start();
  }
}
