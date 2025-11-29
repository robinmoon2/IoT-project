#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_BME680.h>
#include "Seeed_TMG3993.h"
#include "DataStruct.h" // Assure-toi que DataStruct.h est bien dans /include

class SensorManager {
public:
    SensorManager();
    bool begin();
    void readAll(DataStruct& dataOut);

private:
    SPIClass* _vspi;       // Pointeur pour gérer l'instance SPI dynamique
    Adafruit_BME680 _bme;
    TMG3993 _tmg;
};
