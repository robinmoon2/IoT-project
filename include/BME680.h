// BME.h
#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_BME680.h>

extern SPIClass* vspi;
extern Adafruit_BME680 bme;

void configurationBME();
void getDataBME();