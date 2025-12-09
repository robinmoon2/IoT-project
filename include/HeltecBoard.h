#pragma once
#include <Arduino.h>

// to avoid issue we add the RadioLib here 
#include <RadioLib.h> 

// --- Extern declaration ---
// We can use this objects in LoRa.cpp and defined them later in the main.cpp
extern SX1262 radio;
extern int _radiolib_status;
extern void heltec_led(int percent);
extern void heltec_deep_sleep(int ms = 0);
extern bool heltec_wakeup_was_timer();

// --- MACROS ---
// we copy paste the important functions of the Heltec library to use them in LoRa.cpp without to include all the library


#ifndef RADIOLIB_OR_HALT
  #define RADIOLIB_OR_HALT(action) { \
    int _status = action; \
    if (_status != RADIOLIB_ERR_NONE) { \
      Serial.printf("[RadioLib] Error %d\n", _status); \
      while (true); \
    } \
  }
#endif

#ifndef RADIOLIB
  #define RADIOLIB(action) { \
    _radiolib_status = action; \
    if (_radiolib_status != RADIOLIB_ERR_NONE) { \
      Serial.printf("[RadioLib] Error %d\n", _radiolib_status); \
    } \
  }
#endif