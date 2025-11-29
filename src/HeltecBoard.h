#pragma once
#include <Arduino.h>

// On inclut RadioLib ici pour que LoRa.cpp connaisse le type "SX1262"
// La lib Heltec l'utilise en interne.
#include <RadioLib.h> 

// --- DÉCLARATIONS EXTERNES ---
// Cela permet à LoRa.cpp d'utiliser ces objets définis dans main.cpp (via la lib)
extern SX1262 radio;
extern int _radiolib_status;
extern void heltec_led(int percent);
extern void heltec_deep_sleep(int ms = 0);
extern bool heltec_wakeup_was_timer();

// --- MACROS ---
// On recopie les macros pratiques de la lib Heltec pour pouvoir les utiliser dans LoRa.cpp
// sans inclure toute la librairie Heltec qui causerait le conflit.

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