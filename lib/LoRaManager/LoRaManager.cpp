#include "LoRaManager.h"

// Initialisation des membres statiques
volatile bool LoRaManager::_rxFlag = false;

LoRaManager::LoRaManager() : _radio(nullptr), _lastTxTime(0), _minPause(0) {}

void LoRaManager::setRxFlag() {
    _rxFlag = true;
}

void LoRaManager::begin(SX1262* radioModule) {
    _radio = radioModule;
    // Ici on suppose que le module est déjà init physiquement par heltec_setup()
    // On applique juste les param LoRa
    _radio->setFrequency(866.3);
    _radio->setBandwidth(250.0);
    _radio->setSpreadingFactor(9);
    _radio->setOutputPower(0);
    
    // Configuration de l'interruption
    _radio->setDio1Action(LoRaManager::setRxFlag);
    _radio->startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF);
}

void LoRaManager::sendData(const DataStruct& data) {
    // Vérification du temps de pause légal (Duty Cycle)
    if (millis() < _lastTxTime + _minPause) {
        Serial.println("LoRa: Duty Cycle Wait...");
        return;
    }

    std::string msg = "0," + std::to_string(data.temperature) + "," + std::to_string(data.humidity);
    
    // Envoi
    _radio->clearDio1Action();
    uint64_t tStart = millis();
    _radio->transmit(msg.c_str());
    uint64_t duration = millis() - tStart;

    // Calcul pause
    _minPause = duration * 100; // 1% duty cycle
    _lastTxTime = millis();
    
    Serial.printf("LoRa TX: %s (%llu ms)\n", msg.c_str(), duration);

    // Retour en écoute
    _radio->setDio1Action(LoRaManager::setRxFlag);
    _radio->startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF);
}

bool LoRaManager::checkReceive(DataStruct& dataOut) {
    if (_rxFlag) {
        _rxFlag = false;
        String strData;
        _radio->readData(strData);
        
        // Relance écoute
        _radio->startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF);
        
        // Parsing simplifié pour l'exemple
        // (Tu peux remettre ton parser complexe ici)
        Serial.println("LoRa RX: " + strData);
        return true; 
    }
    return false;
}
