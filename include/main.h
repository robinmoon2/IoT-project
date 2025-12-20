#pragma once
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "LittleFS.h"

#include "Config.h"
#include "DataStruct.h"
#include "BME680.h"
#include "TMG3993.h"
#include "LoRa.h"
#define ACTIVATION_PIN 35


const bool MAINBOARD = false;

// Wakeup interval in microseconds (default: 20 minutes)
const uint64_t WAKEUP_INTERVAL_US = 20ULL * 60ULL * 1000000ULL;
bool buttonWake = false;

String receivedataweb ="off";

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

WebServer server(80);
DataStruct data;
