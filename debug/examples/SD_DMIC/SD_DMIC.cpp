/*
 * @Description(CN):
 *      这是一个数字麦克风录音的例程 使用16bit 96kHz 单声道录取声音
 *
 * @Description(EN):
 *      This is an example of recording audio using a digital microphone with 16-bit,
 *  96kHz mono sampling rate.
 *
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-08-17 15:55:47
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-12-23 17:49:43
 * @License: GPL 3.0
 */

#include "DFrobot_MSM261.h"
#include "SPI.h"
#include <SD.h>
#include <FS.h>
#include "pin_config.h"

#define SD_SCLK 2
#define SD_CS 3
#define SD_MOSI 40
#define SD_MISO 41

// 96 KHz
#define MICROPHONE_RECORD_TIME 20    // 采样时间
#define MICROPHONE_SAMPLE_RATE 44100 // 采样速率
#define MICROPHONE_DATA_BIT 16       // 数据位数
// 一秒采集的字节数量 计算方式：采样速率x声道数量x数据位数/8 (可以通过修改该值来修正录音出现噪点的情况)
#define MICROPHONE_BYTERATE (((MICROPHONE_SAMPLE_RATE * MICROPHONE_NUM_CHANNEL * MICROPHONE_DATA_BIT) / 8) + 11550)
#define MICROPHONE_WAVE_DATA_SIZE (MICROPHONE_RECORD_TIME * MICROPHONE_BYTERATE) // 采样字节
#define MICROPHONE_NUM_COMMUNICATION_DATA 500                                    // 每次采集的数据量

#define MICROPHONE_NUM_CHANNEL 2                                                   // 声道数量
#define MICROPHONE_BLOCKALIGN ((MICROPHONE_NUM_CHANNEL * MICROPHONE_DATA_BIT) / 8) // 块对齐方式

#define MICROPHONE_HEADER_SIZE 44

#define MICROPHONE_FILE_NAME "/sound.wav"

byte Wave_Header[MICROPHONE_HEADER_SIZE]; // WAVE文件的文件头
char Wave_CommunicationData[MICROPHONE_NUM_COMMUNICATION_DATA];

DFRobot_Microphone microphone(MSM261_BCLK, MSM261_WS, MSM261_DATA);
File file;

void setup()
{
    Serial.begin(115200);

    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, SPI, 40000000))
        Serial.println("SD begin failed");
    while (!SD.begin(SD_CS, SPI, 40000000))
    {
        Serial.print(".");
        delay(500);
    }

    while (microphone.begin(MICROPHONE_SAMPLE_RATE, MICROPHONE_DATA_BIT) != 0)
    {
        Serial.println(" I2S init failed");
    }
    Serial.println("I2S init success");

    /*
     * @brief 构建WAVE文件的头
     * @param header 构建WAVE的文件
     * @param totalDataLen 需要写入的数据长度
     * @param longSampleRate 采样频率
     * @param channels 声道数量
     * @param byteRate 字节速率 计算方式：采样速率x声道数量x数据位数/8
     * @param blockAlign 块对齐方式 计算方式：声道数量x数据位数/8
    void createWavHeader(byte* header, int totalDataLen, int longSampleRate, uint8_t channels, int byteRate, uint8_t blockAlign);*/
    microphone.createWavHeader(Wave_Header, MICROPHONE_WAVE_DATA_SIZE, MICROPHONE_SAMPLE_RATE,
                               MICROPHONE_NUM_CHANNEL, MICROPHONE_BYTERATE, MICROPHONE_BLOCKALIGN);

    SD.remove(MICROPHONE_FILE_NAME);
    // 打开文件，如果没有文件就创建文件
    file = SD.open(MICROPHONE_FILE_NAME, FILE_WRITE);
    if (!file)
        return;
    // 将wave文件的头写入文件中
    file.write(Wave_Header, MICROPHONE_HEADER_SIZE);
    Serial.println("start");
    for (int j = 0; j < MICROPHONE_WAVE_DATA_SIZE / MICROPHONE_NUM_COMMUNICATION_DATA; ++j)
    {
        microphone.read(Wave_CommunicationData, MICROPHONE_NUM_COMMUNICATION_DATA);

        // 单声道数据处理，当使用单声道时需要处理另一个声道的数据
        // 防止对有效声道的干扰产生杂音，当使用两个麦克风组成双声道时可以屏蔽此步骤。
        for (uint32_t i = 0; i < (MICROPHONE_NUM_COMMUNICATION_DATA >> 2); i++)
        {
            // 合并到右声道
            Wave_CommunicationData[(i << 2)] = 0;
            Wave_CommunicationData[(i << 2) + 1] = 0;

            // 合并到左声道
            // Wave_CommunicationData[(i << 2) + 2] = 0;
            // Wave_CommunicationData[(i << 2) + 3] = 0;
        }
        file.write((uint8_t *)Wave_CommunicationData, MICROPHONE_NUM_COMMUNICATION_DATA);
    }
    file.close();
    Serial.println("finish");
}

void loop()
{
}
