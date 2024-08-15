/*
 * @Description:
            DMIC Test
        Print microphone loudness values on computer serial port
 * @Author: LILYGO_L
 * @Date: 2023-08-17 16:24:06
 * @LastEditTime: 2023-12-22 11:43:48
 * @License: GPL 3.0
 */
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"

#define IIS_SAMPLE_RATE 44100 // 采样速率
#define IIS_DATA_BIT 16       // 数据位数

std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_1, MSM261_BCLK, MSM261_WS, MSM261_DATA);

std::unique_ptr<Arduino_IIS> MSM261(new Arduino_MEMS(IIS_Bus));

char IIS_Read_Buff[100];

void setup()
{
    Serial.begin(115200);

    while (MSM261->begin(Arduino_IIS_DriveBus::Device_Data_Mode::DATA_IN,
                         IIS_SAMPLE_RATE, IIS_DATA_BIT) == false)
    {
        Serial.println("MSM261 initialization fail");
        delay(2000);
    }
    Serial.println("MSM261 initialization successfully");
}

void loop()
{
    if (MSM261->IIS_Read_Data(IIS_Read_Buff, 100) == true)
    {
        // 输出右声道数据
        // Serial.printf("Right: %d\n", (int16_t)(IIS_Read_Buff[2] | IIS_Read_Buff[3] << 8));

        // 输出左声道数据
        // Serial.printf("Left: %d\n", (int16_t)(IIS_Read_Buff[0] | IIS_Read_Buff[1] << 8));

        Serial.print((int16_t)(IIS_Read_Buff[2] | IIS_Read_Buff[3] << 8)); // Arduino
        Serial.print(",");
        Serial.println((int16_t)(IIS_Read_Buff[0] | IIS_Read_Buff[1] << 8));
    }
    else
    {
        Serial.printf("Failed to read MSM261 data");
    }

    delay(50);
}
