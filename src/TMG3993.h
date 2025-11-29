// TMG3993Sensor.h
#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "Seeed_TMG3993.h"

extern TMG3993 tmg3993;

void configurationTMG3993();
void getDataTMG3993();
