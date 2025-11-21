#pragma once

#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include "Arduino.h"

#define HELTEC_POWER_BUTTON   // must be before "#include <heltec_unofficial.h>"
#define WAKEUP_GPIO    GPIO_NUM_14

#include <heltec_unofficial.h>
#define PAUSE               300
#define FREQUENCY           866.3       // for Europe
#define BANDWIDTH           250.0
#define SPREADING_FACTOR    9
#define TRANSMIT_POWER      0
#define ACTIVATION_PIN      35


String rxdata;
volatile bool rxFlag = false;
long counter = 0;
uint64_t last_tx = 0;
uint64_t tx_time;
uint64_t minimum_pause;

using namespace std;

bool buttonWake = false;
bool clockWake  = false;
uint32_t lastPress = 0;


void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();
  

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using LORA"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }

}


void rx() {
  rxFlag = true;
}

void configurationLoRa() {
  heltec_setup();
  pinMode(ACTIVATION_PIN,INPUT);
  both.println("Radio init");
  RADIOLIB_OR_HALT(radio.begin());
  // Set the callback function for received packets
  radio.setDio1Action(rx);
  // Set radio parameters
  both.printf("Frequency: %.2f MHz\n", FREQUENCY);
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));
  both.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));
  both.printf("Spreading Factor: %i\n", SPREADING_FACTOR);
  RADIOLIB_OR_HALT(radio.setSpreadingFactor(SPREADING_FACTOR));
  both.printf("TX power: %i dBm\n", TRANSMIT_POWER);
  RADIOLIB_OR_HALT(radio.setOutputPower(TRANSMIT_POWER));
  // Start receiving
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
}

void SendLoRa(int c ) {
  heltec_loop();
  bool tx_legal = millis() > last_tx + minimum_pause;
// Transmit a packet every PAUSE seconds or when the button is pressed
  if (!tx_legal) {
    both.printf("Legal limit, wait %i sec.\n", (int)((minimum_pause - (millis() - last_tx)) / 1000) + 1);
    return;
  }

  both.printf("TX [%s] ", String(c).c_str());
  radio.clearDio1Action();
  heltec_led(50);

  tx_time = millis();
  RADIOLIB(radio.transmit(String(c).c_str())); // transmit the packages
  tx_time = millis() - tx_time;
  Serial.printf("SENDING : %s",String(c).c_str());

  heltec_led(0); // turn down the LED of the board

  if (_radiolib_status == RADIOLIB_ERR_NONE) {
    both.printf("OK (%i ms)\n", (int)tx_time);
  } else {
    both.printf("fail (%i)\n", _radiolib_status);
  }

  minimum_pause = tx_time * 100;
  last_tx = millis();
  radio.setDio1Action(rx); // go to the receive mode
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
}


DataStruct StringParser(string message){
  DataStruct receivedData;
  string value = "";
  int data_index =0;
  string index ="";
  for(int i=0; i<message.length();i++){
    if(message[i] != ';')
      value += message[i];
    else{
      if(data_index ==0){
      receivedData.temperature = std::stof(value);
      }
      if(data_index == 1){
      receivedData.pressure = std::stof(value);
      }
      if(data_index == 2){
      receivedData.humidity = std::stof(value);
      }
      if(data_index == 3){
      receivedData.light_intensity = std::stof(value);
      }
      data_index ++;
      value = "";
    }
  }
  return receivedData;
}

DataStruct ReceiveLoRa(){ 
  if (rxFlag) {
    rxFlag = false;
    radio.readData(rxdata);
    Serial.println("FLAG");
    if (_radiolib_status == RADIOLIB_ERR_NONE) {
      both.printf("RX [%s]\n", rxdata.c_str());
      both.printf("  RSSI: %.2f dBm\n", radio.getRSSI());
      both.printf("  SNR: %.2f dB\n", radio.getSNR());
    }
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
    return StringParser(rxdata.c_str());
  }
}

