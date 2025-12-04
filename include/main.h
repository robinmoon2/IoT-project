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


const bool MAINBOARD = true;

bool buttonWake = false;

String receivedataweb ="off";

const char* ssid = "Robinou";
const char* password = "robinestbeau";

WebServer server(80);
DataStruct data;
