#pragma once

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
#include "LoRa.hpp"


char buf[30];
const char* ssid = "TelRobin";
const char* password = "robinestbeau";
const bool ISRECEIVER = false; // MODIFY TO MODIFY THE TYPE OF THE LoRA

struct DataStruct{
  float temperature;
  float pressure;
  float humidity;
  float light_intensity;
};


