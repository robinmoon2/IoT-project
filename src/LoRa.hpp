#pragma once

#include "LoRaWan_APP.h"
#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include "LoRaParameters.h"


String concatenateMessageData(DataStruct data){
    String data_message = "";
    data_message = ("%2f;%2f;%2f;%2f",data.temperature,data.pressure,data.humidity,data.light_intensity);
    return data_message;
}

void send_data(DataStruct data){
	if(lora_idle == true)
	{
    String message = concatenateMessageData(data);
    delay(1000);
    sprintf(txpacket,"%s\n",message);
    Serial.printf("packet message : %s", txpacket);
    Serial.printf("\r\nsending packet \"%s\" , length %d\r\n",txpacket, strlen(txpacket));

    Radio.Send( (uint8_t *)txpacket, strlen(txpacket) ); //send the package out
    lora_idle = false;
	}
  Radio.IrqProcess( );
}

void setupLoRaSender() {
    Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);

    txNumber=0;

    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;

    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
}



void OnTxDone( void )
{
	Serial.println("TX done......");
	lora_idle = true;
}

void OnTxTimeout( void )
{
    Radio.Sleep( );
    Serial.println("TX Timeout......");
    lora_idle = true;
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    rssi=rssi;
    rxSize=size;
    memcpy(rxpacket, payload, size );
    rxpacket[size]='\0';
    Radio.Sleep( );
    Serial.printf("\r\nreceived packet \"%s\" with rssi %d , length %d\r\n",rxpacket,rssi,rxSize);
}

void setupLoRaReceiver() {
    Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);

    txNumber=0;
    rssi=0;

    RadioEvents.RxDone = OnRxDone;
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                               LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                               LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                               0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
}

void receiveDataLoRa()
{
  if(lora_idle)
  {
    lora_idle = false;
    Serial.println("into RX mode");
    Radio.Rx(0);
  }
  Radio.IrqProcess( );
}

