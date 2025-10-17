#pragma once
#include <Arduino.h>
#include <Wire.h>

#include "Seeed_TMG3993.h"
#define I2C_SDA 41
#define I2C_SCL 42

TMG3993 tmg3993(0x39,&Wire1);


void configurationTMG3993(){
    Wire1.begin(I2C_SDA, I2C_SCL);
    if (tmg3993.initialize() == false) {
        Serial.println("Device not found. Check wiring.");
        while (1);
    }
    tmg3993.setADCIntegrationTime(0xdb); // the integration time: 103ms
    tmg3993.enableEngines(ENABLE_PON | ENABLE_AEN | ENABLE_AIEN);
}


void getDataTMG3993(){
    if (tmg3993.getSTATUS() & STATUS_AVALID) {
        uint16_t r, g, b, c;
        int32_t lux, cct;
        tmg3993.getRGBCRaw(&r, &g, &b, &c);
        lux = tmg3993.getLux(r, g, b, c);
        //the calculation of CCT is just from the `Application Note`,
        //from the result of our test, it might have error.
        cct = tmg3993.getCCT(r, g, b, c);

        Serial.print("RGBC Data: ");
        Serial.print(r);
        Serial.print("\t");
        Serial.print(g);
        Serial.print("\t");
        Serial.print(b);
        Serial.print("\t");
        Serial.println(c);

        Serial.print("Lux: ");
        Serial.print(lux);
        Serial.print("\tCCT: ");
        Serial.println(cct);
        Serial.println("----");

        // don't forget to clear the interrupt bits
        tmg3993.clearALSInterrupts();
    }
    delay(100);
}
