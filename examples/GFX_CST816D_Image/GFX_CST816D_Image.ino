/*
 * @Description:
            Screen color touch test
        print 16 bit color images on the screen, touch the screen to switch images and print
    touch coordinates
 * @Author: LILYGO_L
 * @Date: 2023-09-06 10:58:19
 * @LastEditTime: 2024-08-15 11:59:07
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include "Material_16Bit_160x160px.h"
#include "Arduino_DriveBus_Library.h"

static uint8_t Touch_Count = 0;

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

    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH);

    ledcAttachPin(LCD_BL, 1);
    ledcSetup(1, 2000, 8);
    ledcWrite(1, 0);

    while (CST816D->begin() == false)
    {
        Serial.println("CST816D initialization fail");
        delay(2000);
    }
    Serial.println("CST816D initialization successfully");

    // 检测到触摸状态变化时，发出低脉冲
    CST816D->IIC_Write_Device_State(CST816D->Arduino_IIC_Touch::Device::TOUCH_DEVICE_INTERRUPT_MODE,
                                    CST816D->Arduino_IIC_Touch::Device_Mode::TOUCH_DEVICE_INTERRUPT_CHANGE);

    // 目前休眠功能只能进入不能退出 所有不建议开启休眠
    // CST816D->IIC_Write_Device_State(CST816D->Arduino_IIC_Touch::Device::TOUCH_DEVICE_SLEEP_MODE,
    //                                 CST816D->Arduino_IIC_Touch::Device_State::TOUCH_DEVICE_ON);

    Serial.printf("ID: %#X \n\n", (int32_t)CST816D->IIC_Device_ID());

    gfx->begin();
    gfx->fillScreen(WHITE);

    gfx->setCursor(20, 100);
    gfx->setTextColor(ORANGE);
    gfx->println("Ciallo1~(L *##*L)^**");

    delay(1000);
}

void loop()
{
    if (CST816D->IIC_Interrupt_Flag == true)
    {
        CST816D->IIC_Interrupt_Flag = false;

        int32_t touch_x = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
        int32_t touch_y = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
        uint8_t fingers_number = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

        if (fingers_number > 0)
        {
            Serial.printf("\nGesture:%s\n",
                          (CST816D->IIC_Read_Device_State(CST816D->Arduino_IIC_Touch::Status_Information::TOUCH_GESTURE_ID)).c_str());
            Serial.printf("Fingers Number:%d\n", fingers_number);
            Serial.printf("Touch X:%d Y:%d\n\n", touch_x, touch_y);

            switch (Touch_Count)
            {
            case 0:
                gfx->fillScreen(RED);
                break;
            case 1:
                gfx->fillScreen(GREEN);
                break;
            case 2:
                gfx->fillScreen(BLUE);
                break;
            case 3:
                gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_1, 160, 160); // RGB
                break;
            case 4:
                gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_2, 160, 160); // RGB
                break;
            case 5:
                gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_3, 160, 160); // RGB
                break;

            default:
                break;
            }

            gfx->setTextColor(PURPLE);
            gfx->setCursor(touch_x, touch_y);
            gfx->printf("Touch X:%d Y:%d \n", touch_x, touch_y);

            Touch_Count++;
            if (Touch_Count > 5)
            {
                Touch_Count = 0;
            }
        }
    }
    // delay(1000);
}
