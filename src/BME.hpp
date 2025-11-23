#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#define BME_SCK 36
#define BME_MISO 37
#define BME_MOSI 35
#define BME_CS 34
#define VSPI FSPI
#define SEALEVELPRESSURE_HPA (1013.25)

SPIClass* vspi = new SPIClass(VSPI);

Adafruit_BME680 bme(BME_CS, vspi);
//Adafruit_BME680 bme(BME_CS,BME_MOSI,BME_MISO, BME_SCK);
//static const int spiClk = 1000000;  // 1 MHz

void configurationBME(){
    if(!bme.begin()){
        Serial.println("Could not find the BME 680. Check the wiring or the address configuration");
        while(1);
    }
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320,150); // 320 *C for 150 ms
}
/*
void spiCommand(SPIClass *spi, byte data) {
  //use it as you would the regular arduino SPI API
  spi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(spi->pinSS(), LOW);  //pull SS slow to prep other end for transfer
  spi->transfer(data);
  digitalWrite(spi->pinSS(), HIGH);  //pull ss high to signify end of data transfer
  spi->endTransaction();
}
*/

void getDataBME(){
    if (! bme.performReading()) {
      Serial.println("Failed to perform reading :(");
      return;
    }
    Serial.print("Temperature = ");
    Serial.print(bme.temperature);
    Serial.println(" *C");

    Serial.print("Pressure =");
    Serial.print(bme.pressure/100.0);
    Serial.println(" hPa");
}