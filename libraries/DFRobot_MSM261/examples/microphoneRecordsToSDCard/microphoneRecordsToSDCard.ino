/*
 * @Description: None
 * @version: V1.0.0
 * @Author: None
 * @Date: 2023-08-17 15:54:15
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-11-08 17:48:32
 * @License: GPL 3.0
 */
/*!
 * @file microphoneRecordsToSDCard.ino
 * @brief 这是一个麦克风录音的例程，使用16bit 44.1kHz 单声道录取声音，运行用例将
 * @n 会录制20秒的声音到SD卡中
 * @copyright  Copyright (c) 2022 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [TangJie](jie.tang@dfrobot.com)
 * @version V1.0
 * @date 2022-02-24
 * @url https://github.com/DFRobot/DFRobot_MSM261
 */

#include "DFrobot_MSM261.h"
#include "SPI.h"
#include <SD.h>
#include <FS.h>
#define SAMPLE_RATE     (44100)
#define I2S_SCK_IO      (25)
#define I2S_WS_IO       (16)
#define I2S_DI_IO       (27)
#define DATA_BIT        (16)
#define MODE_PIN        (4)

const int record_time = 20;  // 采样时间
const char filename[] = "/sound.wav";//保存的文件名称

const int headerSize = 44;
const int byteRate = 176400;//一秒采集的字节数量 计算方式：采样速率x声道数量x数据位数/8
const int waveDataSize = record_time * byteRate;//10秒采样字节
const int numCommunicationData = 512;//每次采集的数据量
byte header[headerSize];//WAVE文件的文件头
char communicationData[numCommunicationData];


DFRobot_Microphone microphone(I2S_SCK_IO, I2S_WS_IO, I2S_DI_IO);
File file;

void setup() {
  Serial.begin(115200);
  pinMode(MODE_PIN,OUTPUT);
  digitalWrite(MODE_PIN,LOW);//将麦克风配置为接收左声道数据
  //digitalWrite(MODE_PIN,HIGH);//将麦克风配置为接收右声道数据
  while(microphone.begin(SAMPLE_RATE, DATA_BIT) != 0){
      Serial.println(" I2S init failed");
  }
  Serial.println("I2S init success");
  if (!SD.begin()) Serial.println("SD begin failed");
    while(!SD.begin()){
      Serial.print(".");
      delay(500);
    }
  /*
   * @brief 构建WAVE文件的头
   * @param header 构建WAVE的文件
   * @param totalDataLen 需要写入的数据长度
   * @param longSampleRate 采样频率
   * @param channels 声道数量
   * @param byteRate 字节速率 计算方式：采样速率x声道数量x数据位数/8
   * @param blockAlign 块对齐方式 计算方式：声道数量x数据位数/8
  void createWavHeader(byte* header, int totalDataLen, int longSampleRate, uint8_t channels, int byteRate, uint8_t blockAlign);*/
  microphone.createWavHeader(header, waveDataSize,44100,2,byteRate,4);
  SD.remove(filename);
  //打开文件，如果没有文件就创建文件
  file = SD.open(filename, FILE_WRITE);
  if (!file) return;
  //将wave文件的头写入文件中
  file.write(header, headerSize);
  Serial.println("start");
  for (int j = 0; j < waveDataSize/numCommunicationData; ++j) {
    microphone.read(communicationData, numCommunicationData);
    /**
     * @brief 单声道数据处理，当使用单声道时需要处理另一个声道的数据
     * 防止对有效声道的干扰产生杂音，当使用两个麦克风组成双声道时可以
     * 屏蔽此步骤。
     */
    for(uint32_t i = 0;i < (numCommunicationData>>2);i++){
      communicationData[(i<<2)] = 0;
      communicationData[(i<<2)+1] = 0;
      //communicationData[(i<<2)+2] = 0;
      //communicationData[(i<<2)+3] = 0;
    }
    file.write((uint8_t*)communicationData, numCommunicationData);
  }
  file.close();
  Serial.println("finish");
}

void loop() {
}
