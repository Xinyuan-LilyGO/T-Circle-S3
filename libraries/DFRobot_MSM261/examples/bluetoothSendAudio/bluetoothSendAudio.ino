/*!
 * @file bluetoothSendAudio.ino
 * @brief 这是一个蓝牙麦克风的用例，只传输左声道数据，运行用例可以实行蓝牙麦克风传输数据
 * @copyright  Copyright (c) 2022 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [TangJie](jie.tang@dfrobot.com)
 * @version V1.0
 * @date 2022-02-18
 * @url https://github.com/DFRobot/DFRobot_MSM261
 */

#include "DFrobot_MSM261.h"
#include "DFRobot_BluetoothA2DPSource.h"


#define SAMPLE_RATE     (44100)
#define I2S_SCK_IO      (25)
#define I2S_WS_IO       (16)
#define I2S_DI_IO       (27)
#define DATA_BIT        (16)
#define MODE_PIN        (4)

DFRobot_Microphone microphone(I2S_SCK_IO, I2S_WS_IO, I2S_DI_IO);
DFRobot_BluetoothA2DPSource a2dp;
const char* bleName = "KFC";//需要链接的蓝牙从机名称
char communicationData[512];
static int32_t btAppA2dDataCb(uint8_t *data, int32_t len)
{   
    if (len < 0 || data == NULL) {
        return 0;
    }
    microphone.read(communicationData, len);
    /**
     * @brief 单声道数据处理，当使用单声道时需要处理另一个声道的数据
     * 防止对有效声道的干扰产生杂音，当使用两个麦克风组成双声道时可以
     * 屏蔽此步骤。
     */
    for(uint32_t i = 0;i < (len>>2);i++){
      communicationData[(i<<2)] = 0;
      communicationData[(i<<2)+1] = 0;
      //communicationData[(i<<2)+2] = 0;
      //communicationData[(i<<2)+3] = 0;
    }
    for (int i = 0; i < (len >> 1); i++) {
        data[(i << 1)] = communicationData[i<<1];
        data[(i << 1) + 1] = communicationData[(i<<1)+1];
    }
    return len;
}

void setup() {
  Serial.begin(115200);
  pinMode(MODE_PIN,OUTPUT);
  digitalWrite(MODE_PIN,LOW);//将麦克风配置为接收左声道数据
  //digitalWrite(MODE_PIN,HIGH);//将麦克风配置为接收右声道数据
  while(microphone.begin(SAMPLE_RATE, DATA_BIT) != 0){
      Serial.println(" I2S init failed");
  }
  Serial.println("I2S init success");
  while(!a2dp.begin(bleName,btAppA2dDataCb,50)){
    Serial.println(" a2dp init failed");
  }
}

void loop() {
}
