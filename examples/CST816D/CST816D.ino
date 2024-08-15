/*
 * @Description: 
            CST816D Test
        Print touch coordinate values on the computer serial port
 * @Author: LILYGO_L
 * @Date: 2024-01-25 11:52:57
 * @LastEditTime: 2024-08-15 11:53:53
 * @License: GPL 3.0
 */
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"

static size_t CycleTime = 0;

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

    while (CST816D->begin() == false)
    {
        Serial.println("CST816D initialization fail");
        delay(2000);
    }
    Serial.println("CST816D initialization successfully");

    // 中断模式为检测到触摸时，发出低脉冲
    CST816D->IIC_Write_Device_State(CST816D->Arduino_IIC_Touch::Device::TOUCH_DEVICE_INTERRUPT_MODE,
                                    CST816D->Arduino_IIC_Touch::Device_Mode::TOUCH_DEVICE_INTERRUPT_PERIODIC);

    // 目前休眠功能只能进入不能退出 所有不建议开启休眠
    // CST816D->IIC_Write_Device_State(CST816D->Arduino_IIC_Touch::Device::TOUCH_DEVICE_SLEEP_MODE,
    //                                 CST816D->Arduino_IIC_Touch::Device_State::TOUCH_DEVICE_ON);

    Serial.printf("ID: %#X \n\n", (int32_t)CST816D->IIC_Device_ID());
    delay(1000);
}

void loop()
{
    // Serial.printf("System running time: %d\n\n", (uint32_t)millis() / 1000);

    if (CycleTime < millis())
    {
        std::vector<uint8_t> iic_device_address;
        if (IIC_Bus->IIC_Device_7Bit_Scan(&iic_device_address) == true)
        {
            for (int i = 0; i < iic_device_address.size(); i++)
            {
                Serial.printf("Find IIC_ID[%d]: 0x", i);
                if (iic_device_address[i] < 0x10)
                {
                    Serial.print("0");
                    Serial.print(iic_device_address[i], HEX);
                }
                else
                {
                    Serial.print(iic_device_address[i], HEX);
                }
                Serial.println();
            }
        }
        CycleTime = millis() + 1000;
    }

    if (CST816D->IIC_Interrupt_Flag == true)
    {
        CST816D->IIC_Interrupt_Flag = false;

        Serial.printf("\nGesture:%s\n",
                      (CST816D->IIC_Read_Device_State(CST816D->Arduino_IIC_Touch::Status_Information::TOUCH_GESTURE_ID)).c_str());
        Serial.printf("Fingers Number:%d\n",
                      (int32_t)CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER));
        Serial.printf("Touch X:%d Y:%d\n\n",
                      (int32_t)CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X),
                      (int32_t)CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y));
    }

    delay(100);
}
