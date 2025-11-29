#pragma once
#include <Arduino.h>
#include <string>
#include "DataStruct.h"

// Fonctions utilitaires
void print_wakeup_reason();

// Fonctions LoRa
void configurationLoRa();
void SendLoRa(int c);
void SendLoRa(DataStruct data); // Passage par valeur pour correspondre à ton code
DataStruct ReceiveLoRa();
int ReceiveLoRaWakeUp();
