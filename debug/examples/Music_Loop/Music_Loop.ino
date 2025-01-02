/*
 * @Description: None
 * @Author: LILYGO_L
 * @Date: 2024-12-17 09:59:45
 * @LastEditTime: 2024-12-17 12:13:05
 * @License: GPL 3.0
 */
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"
#include "c2_b16_s44100.h"

// 44.1 KHz
#define IIS_SAMPLE_RATE 44100 // 采样速率
#define IIS_DATA_BIT 16       // 数据位数

std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus_1 =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_1, MAX98357A_BCLK, MAX98357A_LRCLK,
                                    MAX98357A_DATA);

std::unique_ptr<Arduino_IIS> MAX98357A(new Arduino_Amplifier(IIS_Bus_1));

void setup()
{
    Serial.begin(115200);
    pinMode(MAX98357A_SD_MODE, OUTPUT);
    digitalWrite(MAX98357A_SD_MODE, HIGH);

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
    if (MAX98357A->IIS_Write_Data(c2_b16_s44100, sizeof(c2_b16_s44100)) == true)
    {
        // Serial.printf("MAX98357A played successfully\n");
    }

    // delay(1000);
}