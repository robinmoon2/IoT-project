#pragma once
#include <Arduino.h>
#include <RadioLib.h>
#include <string>
#include "DataStruct.h"

class LoRaManager {
public:
    LoRaManager(); // Constructeur

    // Configuration
    void begin(SX1262* radioModule);
    
    // Actions
    void sendData(const DataStruct& data);
    void sendPing(int id);
    bool checkReceive(DataStruct& dataOut); // Retourne true si data reçue

    // Callback pour l'interruption (doit être statique ou géré en externe)
    static void setRxFlag(); 

private:
    SX1262* _radio;
    static volatile bool _rxFlag; // Static pour fonctionner avec l'ISR
    
    // Variables de gestion du temps
    uint64_t _lastTxTime;
    uint64_t _minPause;

    // Méthodes internes
    DataStruct parseString(std::string input);
};
