/*
 * @Description: 
            Example of microphone speaker
        Re output the microphone input data from the speaker
 * @Author: LILYGO_L
 * @Date: 2023-12-21 11:30:50
 * @LastEditTime: 2024-08-15 13:30:56
 * @License: GPL 3.0
 */
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"

// 44.1 KHz
#define IIS_SAMPLE_RATE 44100 // 采样速率
#define IIS_DATA_BIT 16       // 数据位数

char IIS_Read_Buff[100];

std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus_0 =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_0, MSM261_BCLK, MSM261_WS, MSM261_DATA);

std::unique_ptr<Arduino_IIS> MSM261(new Arduino_MEMS(IIS_Bus_0));

std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus_1 =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_1, MAX98357A_BCLK, MAX98357A_LRCLK,
                                    MAX98357A_DATA);

std::unique_ptr<Arduino_IIS> MAX98357A(new Arduino_Amplifier(IIS_Bus_1));

void setup()
{
    Serial.begin(115200);
    pinMode(MAX98357A_SD_MODE, OUTPUT);
    digitalWrite(MAX98357A_SD_MODE, HIGH);

    while (MSM261->begin(Arduino_IIS_DriveBus::Device_Data_Mode::DATA_IN,
                         IIS_SAMPLE_RATE, IIS_DATA_BIT) == false)
    {
        Serial.println("MSM261 initialization fail");
        delay(2000);
    }
    Serial.println("MSM261 initialization successfully");

    while (MAX98357A->begin(Arduino_IIS_DriveBus::Device_Data_Mode::DATA_OUT,
                            IIS_SAMPLE_RATE, IIS_DATA_BIT) == false)
    {
        Serial.println("MAX98357A initialization fail");
        delay(2000);
    }
    Serial.println("MAX98357A initialization successfully");

    // MSM261->IIS_Device_Switch(Arduino_IIS::Device_Switch::Channel_OFF);
    // MAX98357A->IIS_Device_Switch(Arduino_IIS::Device_Switch::Channel_OFF);
}

void loop()
{
    if (MSM261->IIS_Read_Data(IIS_Read_Buff, 10) == true)
    {
        // 输出左声道数据
        // Serial.printf("Left: %d\n", (int16_t)((int16_t)IIS_Read_Buff[0] | (int16_t)IIS_Read_Buff[1] << 8));

        // 输出右声道数据
        // Serial.printf("Right: %d\n", (int16_t)((int16_t)IIS_Read_Buff[2] | (int16_t)IIS_Read_Buff[3] << 8));

        if (MAX98357A->IIS_Write_Data(IIS_Read_Buff, 10) == true)
        {
            // Serial.printf("MAX98357A played successfully\n");
        }

        // Arduino
        // Serial.println((int16_t)((int16_t)IIS_Read_Buff[0] | (int16_t)IIS_Read_Buff[1] << 8));
        // Serial.print(",");
        // Serial.print((int16_t)((int16_t)IIS_Read_Buff[2] | (int16_t)IIS_Read_Buff[3] << 8));
    }
    else
    {
        Serial.printf("Failed to read MSM261 data");
    }

    // delay(1);
}