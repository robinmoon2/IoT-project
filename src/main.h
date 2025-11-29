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

bool buttonWake = false;

String receivedataweb ="off";

const char* ssid = "A54cluzet";
const char* password = "alexandre2004";

WebServer server(80);
DataStruct data;
