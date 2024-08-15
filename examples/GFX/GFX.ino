/*
 * @Description: 
            GFX Test
        Screen testing
 * @Author: LILYGO_L
 * @Date: 2023-09-06 10:58:19
 * @LastEditTime: 2024-08-15 13:01:30
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"

Arduino_DataBus *bus = new Arduino_ESP32SPIDMA(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */);

Arduino_GFX *gfx = new Arduino_GC9D01N(
    bus, LCD_RST /* RST */, 0 /* rotation */, false /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH);

    ledcAttachPin(LCD_BL, 1);
    ledcSetup(1, 2000, 8);
    ledcWrite(1, 0);

    gfx->begin();
    gfx->fillScreen(WHITE);
    delay(1000);
    gfx->fillScreen(RED);
    delay(1000);
    gfx->fillScreen(GREEN);
    delay(1000);
    gfx->fillScreen(BLUE);
    delay(1000);
    gfx->fillScreen(PINK);

    gfx->setCursor(20, 100);
    gfx->setTextColor(YELLOW);
    gfx->println("Ciallo1~(L *##*L)^**");

    delay(1000);
    gfx->fillScreen(PINK);

    gfx->drawRect(40, 40, 80, 80, RED);
    gfx->drawFastVLine(80, 40, 80, PURPLE);
    gfx->drawFastHLine(40, 80, 80, PURPLE);

    // for (int i = 0; i < 100; i++)
    // {
    //     gfx->drawPixel(40 , 40+ i, RED);
    //     // delay(100);
    // }
}

void loop()
{
    // gfx->fillScreen(PINK);
    delay(1000);
}
