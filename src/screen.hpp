#pragma once
#include <heltec.h>
#include <Arduino.h>
#include <Wire.h>
#include "HT_SSD1306Wire.h"

static SSD1306Wire  display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

void InitScreen()
{

    display.init();
    display.setFont(ArialMT_Plain_10);
}
