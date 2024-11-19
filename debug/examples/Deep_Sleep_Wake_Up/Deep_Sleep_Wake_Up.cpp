/*
 * @Description: ESP Sleep
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-08-18 15:26:31
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2024-03-19 16:52:45
 * @License: GPL 3.0
 */
#include "Arduino.h"
#include "pin_config.h"
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"

#define SLEEP_WAKE_UP_INT GPIO_NUM_0
#define SLEEP_WAKE_UP_LCD_BL GPIO_NUM_18

static size_t CycleTime = 0;

Arduino_DataBus *bus = new Arduino_ESP32SPIDMA(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */);

Arduino_GFX *gfx = new Arduino_GC9D01N(
    bus, LCD_RST /* RST */, 0 /* rotation */, false /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

void Arduino_IIC_Touch_Interrupt(void);

std::unique_ptr<Arduino_IIC> CST816D(new Arduino_CST816x(IIC_Bus, CST816D_DEVICE_ADDRESS,
                                                         TP_RST, TP_INT, Arduino_IIC_Touch_Interrupt));

void Arduino_IIC_Touch_Interrupt(void)
{
    CST816D->IIC_Interrupt_Flag = true;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(SLEEP_WAKE_UP_INT, INPUT_PULLUP);

    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, LOW);

    pinMode(MAX98357A_SD_MODE, OUTPUT);
    digitalWrite(MAX98357A_SD_MODE, LOW);

    if (CST816D->begin() == false)
    {
        Serial.println("CST816D initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("CST816D initialization successfully");
    }

    // 中断模式为检测到触摸时，发出低脉冲
    CST816D->IIC_Write_Device_State(CST816D->Arduino_IIC_Touch::Device::TOUCH_DEVICE_INTERRUPT_MODE,
                                    CST816D->Arduino_IIC_Touch::Device_Mode::TOUCH_DEVICE_INTERRUPT_PERIODIC);

    gfx->begin();
    gfx->fillScreen(WHITE);

    gfx->setCursor(10, 70);
    gfx->setTextColor(RED);
    gfx->setTextSize(1);
    gfx->println("Enter deep sleep in 5 seconds");

    Serial.println("Enter deep sleep in 5 seconds");

    CycleTime = millis() + 5000;
}

void loop()
{
    if (millis() > CycleTime)
    {
        delay(300);

        Serial.println("Enter deep sleep");
        digitalWrite(LCD_BL, HIGH);

        gfx->displayOff();

        // 目前休眠功能只能进入不能退出 所有不建议开启休眠
        CST816D->IIC_Write_Device_State(CST816D->Arduino_IIC_Touch::Device::TOUCH_DEVICE_SLEEP_MODE,
                                        CST816D->Arduino_IIC_Touch::Device_State::TOUCH_DEVICE_ON);

        gpio_hold_en(SLEEP_WAKE_UP_INT);
        gpio_hold_en(SLEEP_WAKE_UP_LCD_BL);
        esp_sleep_enable_ext0_wakeup(SLEEP_WAKE_UP_INT, LOW);
        esp_deep_sleep_start();
    }
}
