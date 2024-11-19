/*
 * @Description: None
 * @version: V1.0.0
 * @Author: None
 * @Date: 2023-08-29 13:34:55
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2024-02-02 09:30:40
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "pin_config.h"

uint8_t i = 0;

void setup()
{
    Serial.begin(115200);

    // Class_XL95x5.begin();
    // Class_XL95x5.read_all_reg(); // Read all registers

    // Class_XL95x5.portMode(XL95x5_PORT_0, OUTPUT); // Configure the XL95x5 full port mode
    // Class_XL95x5.portMode(XL95x5_PORT_1, OUTPUT);

    // pinMode(GPIO_NUM_45, OUTPUT);
    // pinMode(GPIO_NUM_46, OUTPUT);
    // pinMode(GPIO_NUM_42, OUTPUT);
    // pinMode(GPIO_NUM_13, OUTPUT);
    // pinMode(GPIO_NUM_12, OUTPUT);
    // pinMode(GPIO_NUM_11, OUTPUT);
    // pinMode(GPIO_NUM_10, OUTPUT);
    // pinMode(GPIO_NUM_9, OUTPUT);
    // pinMode(GPIO_NUM_8, OUTPUT);
    // pinMode(GPIO_NUM_7, OUTPUT);
    // pinMode(GPIO_NUM_6, OUTPUT);
    // pinMode(GPIO_NUM_5, OUTPUT);
    // pinMode(GPIO_NUM_4, OUTPUT);
    // pinMode(GPIO_NUM_3, OUTPUT);
    // pinMode(GPIO_NUM_2, OUTPUT);
    // pinMode(GPIO_NUM_1, OUTPUT);
    // pinMode(GPIO_NUM_45, OUTPUT);
    // pinMode(GPIO_NUM_40, OUTPUT);
    // pinMode(GPIO_NUM_39, OUTPUT);
    // pinMode(GPIO_NUM_41, OUTPUT);
    // pinMode(GPIO_NUM_14, OUTPUT);

    pinMode(GPIO_NUM_47, OUTPUT);
    pinMode(GPIO_NUM_48, OUTPUT);
    // pinMode(GPIO_NUM_15, OUTPUT);
    // pinMode(GPIO_NUM_13, OUTPUT);

    // digitalWrite(GPIO_NUM_45, i);
    // digitalWrite(GPIO_NUM_46, i);
    // digitalWrite(GPIO_NUM_42, i);
    // digitalWrite(GPIO_NUM_13, i);
    // digitalWrite(GPIO_NUM_12, i);
    // digitalWrite(GPIO_NUM_11, i);
    // digitalWrite(GPIO_NUM_10, i);
    // digitalWrite(GPIO_NUM_9, i);
    // digitalWrite(GPIO_NUM_8, i);
    // digitalWrite(GPIO_NUM_7, i);
    // digitalWrite(GPIO_NUM_6, i);
    // digitalWrite(GPIO_NUM_5, i);
    // digitalWrite(GPIO_NUM_4, i);
    // digitalWrite(GPIO_NUM_3, i);
    // digitalWrite(GPIO_NUM_2, i);
    // digitalWrite(GPIO_NUM_1, i);
    // digitalWrite(GPIO_NUM_45, i);
    // digitalWrite(GPIO_NUM_40, i);
    // digitalWrite(GPIO_NUM_39, i);
    // digitalWrite(GPIO_NUM_41, i);
    // digitalWrite(GPIO_NUM_14, i);
    // Class_XL95x5.digitalWrite(XL95x5_GPIO_NUM_15, i);
    // Class_XL95x5.digitalWrite(XL95x5_GPIO_NUM_14, i);
    // Class_XL95x5.digitalWrite(XL95x5_GPIO_NUM_13, i);

    digitalWrite(GPIO_NUM_47, i);
    digitalWrite(GPIO_NUM_48, i);
    // digitalWrite(GPIO_NUM_15, i);
    // digitalWrite(GPIO_NUM_13, i);
}

void loop()
{
    // digitalWrite(GPIO_NUM_45, i);
    // digitalWrite(GPIO_NUM_46, i);
    // digitalWrite(GPIO_NUM_42, i);
    // digitalWrite(GPIO_NUM_13, i);
    // digitalWrite(GPIO_NUM_12, i);
    // digitalWrite(GPIO_NUM_11, i);
    // digitalWrite(GPIO_NUM_10, i);
    // digitalWrite(GPIO_NUM_9, i);
    // digitalWrite(GPIO_NUM_8, i);
    // digitalWrite(GPIO_NUM_7, i);
    // digitalWrite(GPIO_NUM_6, i);
    // digitalWrite(GPIO_NUM_5, i);
    // digitalWrite(GPIO_NUM_4, i);
    // digitalWrite(GPIO_NUM_3, i);
    // digitalWrite(GPIO_NUM_2, i);
    // digitalWrite(GPIO_NUM_1, i);
    // digitalWrite(GPIO_NUM_45, i);
    // digitalWrite(GPIO_NUM_40, i);
    // digitalWrite(GPIO_NUM_39, i);
    // digitalWrite(GPIO_NUM_41, i);
    // digitalWrite(GPIO_NUM_14, i);
    // Class_XL95x5.digitalWrite(XL95x5_GPIO_NUM_15, i);
    // Class_XL95x5.digitalWrite(XL95x5_GPIO_NUM_14, i);
    // Class_XL95x5.digitalWrite(XL95x5_GPIO_NUM_13, i);

    digitalWrite(GPIO_NUM_47, i);
    digitalWrite(GPIO_NUM_48, i);
    // digitalWrite(GPIO_NUM_15, i);
    // digitalWrite(GPIO_NUM_13, i);
    i = !i;
    delay(3000);
}