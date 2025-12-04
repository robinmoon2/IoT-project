#pragma once
#include <Arduino.h>
#include <string>
#include "DataStruct.h"

// Utilitary function 
void print_wakeup_reason();

// Function of Lora
void configurationLoRa();
void SendLoRa(int c);
void SendLoRa(DataStruct data); 
DataStruct ReceiveLoRa();
int ReceiveLoRaWakeUp();
