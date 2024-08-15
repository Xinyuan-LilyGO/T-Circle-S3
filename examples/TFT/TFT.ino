/*
 * @Description:
            GFX Test
        Screen testing
 * @Author: LILYGO_L
 * @Date: 2023-07-25 13:45:02
 * @LastEditTime: 2024-07-25 11:04:28
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "TFT_eSPI.h"
#include "pin_config.h"

TFT_eSPI tft;

void setup(void)
{
    ledcAttachPin(LCD_BL, 1);
    ledcSetup(1, 2000, 8);
    ledcWrite(1, 0);

    tft.begin();
    tft.fillScreen(TFT_WHITE);

    tft.setCursor(10, 80);
    tft.setTextFont(2);
    tft.setTextColor(TFT_BLACK);
    tft.println("Ciallo~(L *##*L)^**");

    tft.drawRect(30, 40, 60, 60, TFT_BLACK);

    tft.drawLine(120, 10, 120, 100, TFT_BLACK);
}

void loop()
{
}