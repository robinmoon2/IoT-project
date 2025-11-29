#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define BME_SCK  6
#define BME_MISO 5
#define BME_MOSI 4
#define BME_CS   3
#define VSPI FSPI
#define SEALEVELPRESSURE_HPA (1013.25)

SPIClass* vspi = new SPIClass(VSPI);
Adafruit_BME680 bme(BME_CS, vspi);

void configurationBME(){
    Serial.println("Initialisation du bus SPI capteur...");
    vspi->begin(BME_SCK, BME_MISO, BME_MOSI, BME_CS);
    pinMode(BME_CS, OUTPUT);
    digitalWrite(BME_CS, HIGH);

    if(!bme.begin()){
        Serial.println(" ERREUR: BME680 introuvable !");
        Serial.println("Vérifiez le câblage :");
        Serial.printf("SCK: %d, MISO: %d, MOSI: %d, CS: %d\n", BME_SCK, BME_MISO, BME_MOSI, BME_CS);

    } else {
        Serial.println(" BME680 trouvé et initialisé !");
        
        bme.setTemperatureOversampling(BME680_OS_8X);
        bme.setHumidityOversampling(BME680_OS_2X);
        bme.setPressureOversampling(BME680_OS_4X);
        bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
        bme.setGasHeater(320, 150); 
    }
}

void getDataBME() {
    if (!vspi) {
      Serial.println("ISSUE VSPI");   
      return;
    }
    if (!bme.performReading()) {
        Serial.println("⚠️ Echec lecture BME (Capteur non prêt ou déconnecté)");
        return;
    }
    Serial.print("Temp: "); Serial.print(bme.temperature); Serial.println(" °C");
    Serial.print("Hum: "); Serial.print(bme.humidity); Serial.println(" %");
    Serial.print("Pres: "); Serial.print(bme.pressure / 100.0); Serial.println(" hPa");   
}
