/*
 * @Description:
            DMIC Test
        Print microphone loudness values on computer serial port
 * @Author: LILYGO_L
 * @Date: 2023-08-17 16:24:06
 * @LastEditTime: 2025-02-14 14:12:52
 * @License: GPL 3.0
 */
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"

#define IIS_SAMPLE_RATE 44100 // 采样速率
#define IIS_DATA_BIT 16       // 数据位数

#ifdef T_Circle_S3_V1_0
std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_0, MSM261_BCLK, MSM261_WS, MSM261_DATA);
#elif defined T_Circle_S3_V1_1
std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_0, -1, MP34DT05TR_LRCLK, MP34DT05TR_DATA);
#else
#error "Unknown macro definition. Please select the correct macro definition."
#endif

std::unique_ptr<Arduino_IIS> Microphone(new Arduino_MEMS(IIS_Bus));

char IIS_Read_Buff[100];

void setup()
{
    Serial.begin(115200);

#ifdef T_Circle_S3_V1_0
    while (Microphone->begin(i2s_mode_t::I2S_MODE_MASTER, ad_iis_data_mode_t::AD_IIS_DATA_IN, i2s_channel_fmt_t::I2S_CHANNEL_FMT_RIGHT_LEFT,
                             IIS_DATA_BIT, IIS_SAMPLE_RATE) == false)
    {
        Serial.println("Microphone initialization fail");
        delay(2000);
    }
#elif defined T_Circle_S3_V1_1
    while (Microphone->begin(i2s_mode_t::I2S_MODE_PDM, ad_iis_data_mode_t::AD_IIS_DATA_IN, i2s_channel_fmt_t::I2S_CHANNEL_FMT_RIGHT_LEFT,
                             IIS_DATA_BIT, IIS_SAMPLE_RATE) == false)
    {
        Serial.println("Microphone initialization fail");
        delay(2000);
    }
#else
#error "Unknown macro definition. Please select the correct macro definition."
#endif

    Serial.println("Microphone initialization successfully");
}

void loop()
{
    if (Microphone->IIS_Read_Data(IIS_Read_Buff, 100) == true)
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
