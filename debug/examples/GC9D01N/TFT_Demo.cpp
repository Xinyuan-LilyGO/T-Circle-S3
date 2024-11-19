/*
 * @Description: None
 * @version: V1.0.0
 * @Author: None
 * @Date: 2024-01-25 09:20:25
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2024-01-25 10:48:34
 * @License: GPL 3.0
 */
#include <TFT_GC9D01N.h>
#include "img.h"

TFT_GC9D01N_Class TFT_099;
char *str = "ABCabc123";

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);

    Serial.println("setup ");
    TFT_099.begin();
    TFT_099.backlight(50);

    // TFT_099.DispColor(0, 0, TFT_WIDTH, TFT_HEIGHT, RED);
    // delay(500);
    // TFT_099.DispColor(0, 0, TFT_WIDTH, TFT_HEIGHT, BLUE);
    // delay(500);
    // TFT_099.DispColor(0, 0, TFT_WIDTH, TFT_HEIGHT, GREEN);
    // delay(500);
    TFT_099.DispColor(0, 0, TFT_WIDTH, TFT_HEIGHT, BLACK);
    delay(500);
    TFT_099.DispStr(str, 0, 0, WHITE, BLACK);
    delay(2000);

    // TFT_099.DispColor2(40, 41, 40, 41, RED);

    for (int i = 0; i < 10; i++)
    {
        TFT_099.BlockWrite(40 + i, 40 + i + 1, 40 - 1, 40);
        TFT_099.WriteOneDot(RED);
        TFT_099.WriteOneDot(BLACK);
        TFT_099.WriteOneDot(BLACK);

        delay(1000);
    }

    // TFT_099.DrawImage(0, 0, 40, 40, closeX);
    // TFT_099.DrawImage(0, 40, 40, 40, closeX);
    // TFT_099.DrawImage(0, 40 * 2,  40, 40, closeX);
    // TFT_099.DrawImage(0, 40 * 3, 40, 40, closeX);

    delay(2000);
}
int i = 0;
void loop()
{
}
