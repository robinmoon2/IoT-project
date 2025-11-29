#include "LoRa.h"
#include "HeltecBoard.h" // Inclut les "extern" et RadioLib

#include <vector>
#include <sstream>
#include <iostream>
#include <string>

// NOTE : On n'inclut PAS <heltec_unofficial.h> ici pour éviter les multiples définitions.

#define FREQUENCY           866.3
#define BANDWIDTH           250.0
#define SPREADING_FACTOR    9
#define TRANSMIT_POWER      0

using namespace std;

String rxdata;
volatile bool rxFlag = false;
uint64_t last_tx = 0;
uint64_t tx_time;
uint64_t minimum_pause = 0;

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
  Serial.println("Radio init");
  RADIOLIB_OR_HALT(radio.begin());
  // Set the callback function for received packets
  radio.setDio1Action(rx);
  // Set radio parameters
  Serial.printf("Frequency: %.2f MHz\n", FREQUENCY);
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));
  Serial.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));
  Serial.printf("Spreading Factor: %i\n", SPREADING_FACTOR);
  RADIOLIB_OR_HALT(radio.setSpreadingFactor(SPREADING_FACTOR));
  Serial.printf("TX power: %i dBm\n", TRANSMIT_POWER);
  RADIOLIB_OR_HALT(radio.setOutputPower(TRANSMIT_POWER));
  // Start receiving
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
}


// ============ SENDING FUNCTION =====================
void SendLoRa(int c) {
  bool tx_legal = millis() > last_tx + minimum_pause;
  // Transmit a packet every PAUSE seconds or when the button is pressed
  if (!tx_legal) {
    Serial.printf("Legal limit, wait %i sec.\n", (int)((minimum_pause - (millis() - last_tx)) / 1000) + 1);
    return;
  }
  Serial.printf("TX [%s] ", String(c).c_str());
  radio.clearDio1Action();
  heltec_led(50);

  tx_time = millis();
  RADIOLIB(radio.transmit(String(c).c_str())); // transmit the packages
  tx_time = millis() - tx_time;
  Serial.printf("SENDING : %s",String(c).c_str());

  heltec_led(0); // turn down the LED of the board

  if (_radiolib_status == RADIOLIB_ERR_NONE) {
    Serial.printf("OK (%i ms)\n", (int)tx_time);
  } else {
    Serial.printf("fail (%i)\n", _radiolib_status);
  }

  minimum_pause = tx_time * 100;
  last_tx = millis();
  radio.setDio1Action(rx); // go to the receive mode
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
}

void SendLoRa(DataStruct data) {
  bool tx_legal = millis() > last_tx + minimum_pause;
  // Transmit a packet every PAUSE seconds or when the button is pressed

  if (!tx_legal) {
    Serial.printf("Legal limit, wait %i sec.\n", (int)((minimum_pause - (millis() - last_tx)) / 1000) + 1);
    return;
  }

  string message = "0,";
  message+= to_string(data.temperature);
  message+=",";
  message+=to_string(data.humidity);

  Serial.print("DATA : ");
  Serial.printf("%s \n", message);

  Serial.printf("TX [%s] ", (message).c_str());
  radio.clearDio1Action();
  heltec_led(50);

  tx_time = millis();
  RADIOLIB(radio.transmit((message).c_str())); // transmit the packages
  tx_time = millis() - tx_time;
  Serial.printf("SENDING : %s",(message).c_str());

  heltec_led(0); // turn down the LED of the board

  if (_radiolib_status == RADIOLIB_ERR_NONE) {
    Serial.printf("OK (%i ms)\n", (int)tx_time);
  } else {
    Serial.printf("fail (%i)\n", _radiolib_status);
  }

  minimum_pause = tx_time * 100;
  last_tx = millis();
  radio.setDio1Action(rx); // go to the receive mode
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
}



// ============ RECEIVING FUNCTION =====================

DataStruct StringParser(string input){

  size_t pos = 0;
  string token;
  string delimiter = ",";
  vector<float> numbers;

  while((pos = input.find(delimiter)) != string::npos){
      token = input.substr(0,pos);
      numbers.push_back((float)atof(token.c_str()));
      input.erase(0,pos + delimiter.length());
  }
  numbers.push_back((float)atof(input.c_str()));

  for(int i=0; i<numbers.size();i++){
    Serial.print(numbers[i]);
    Serial.print(",");
  }
  
  Serial.println();
  DataStruct receivedData;
  receivedData.temperature = numbers[1];
  receivedData.humidity = numbers[2];
  return receivedData; 
  /*regex numberRegex(R"(\d+)");
  sregex_iterator it(input.begin(), input.end(), numberRegex);
  sregex_iterator end;
  vector<float> numbers;

  while (it != end) {
      numbers.push_back(stof(string(it->str())));
      ++it;
  }

  for(int i=0; i<numbers.size();i++){
    Serial.print(numbers[i]);
    Serial.print(",");
  }
  Serial.println();
  DataStruct receivedData;
  receivedData.temperature = numbers[1];
  receivedData.humidity = numbers[2];
  return receivedData; */
}

DataStruct ReceiveLoRa(){ 
  if (rxFlag) {
    rxFlag = false;
    radio.readData(rxdata);
    Serial.println("FLAG");
    if (_radiolib_status == RADIOLIB_ERR_NONE) {
      Serial.printf("RX [%s]\n", rxdata.c_str());
      Serial.printf("  RSSI: %.2f dBm\n", radio.getRSSI());
      Serial.printf("  SNR: %.2f dB\n", radio.getSNR());
    }
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
    return StringParser(rxdata.c_str());
  }
  DataStruct empty{};
  return empty;
}


int ReceiveLoRaWakeUp(){ 
  if (rxFlag) {
    rxFlag = false;
    radio.readData(rxdata);
    Serial.println("FLAG");
    if (_radiolib_status == RADIOLIB_ERR_NONE) {
      Serial.printf("RX [%s]\n", rxdata.c_str());
      Serial.printf("  RSSI: %.2f dBm\n", radio.getRSSI());
      Serial.printf("  SNR: %.2f dB\n", radio.getSNR());
    }
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
    return rxdata.toInt();
  }
  return 0;
}


