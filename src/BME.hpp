#pragma once
/* 
File for the configuration of the BME 680 and the phnomenon that we want

Uses as a global file
*/ 



#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define SEALEVELPRESSURE_HPA (1013.25)
//#define BME_address =  0X76


Adafruit_BME680 bme(&Wire);

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