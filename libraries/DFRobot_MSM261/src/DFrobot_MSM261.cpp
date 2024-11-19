/*!
 * @file DFRobot_Microphone.cpp
 * @brief 这是麦克风库的方法
 * @copyright	Copyright (c) 2021 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [TangJie](jie.tang@dfrobot.com)
 * @version V1.0
 * @date 2022-2-18
 * @url https://github.com/DFRobot/DFRobot_MSM261
 */
#include "DFrobot_MSM261.h"

DFRobot_Microphone::DFRobot_Microphone(uint8_t bckIoNum, uint8_t wsIoNum, uint8_t dInNum)
{
  _bckIoNum = bckIoNum;
  _wsIoNum  = wsIoNum;
  _dInNum   = dInNum;
}

uint8_t DFRobot_Microphone::begin(uint32_t baudRate,uint8_t bit)
{
	uint8_t state=0;
	uint8_t ret=0;
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),                                  
    .sample_rate = baudRate,
    .bits_per_sample = (i2s_bits_per_sample_t)bit,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                          
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
		.intr_alloc_flags = 0 ,
    .dma_buf_count = 16,
    .dma_buf_len = 512,
    .use_apll = false
    };
  i2s_pin_config_t pin_config = {
    .bck_io_num = _bckIoNum,
    .ws_io_num = _wsIoNum,
    .data_out_num = -1,
    .data_in_num = _dInNum                                             
  };
	state = i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  state += i2s_set_pin(I2S_NUM, &pin_config);
	if(state != 0)
		ret = state;
	else
		ret = 0;
  return ret;
}

uint32_t DFRobot_Microphone::read(char* buffer,size_t len)
{
  uint32_t returnDataLen;
  i2s_read(I2S_NUM, (char*)buffer, len, &returnDataLen, 1);
  return returnDataLen;
}

void DFRobot_Microphone::createWavHeader(byte* header, int totalDataLen, int longSampleRate, uint8_t channels, int byteRate, uint8_t blockAlign)
{
  header[0] = 'R';
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';
  unsigned int fileSizeMinus8 = totalDataLen + 44 - 8;
  header[4] = (byte)(fileSizeMinus8 & 0xFF);
  header[5] = (byte)((fileSizeMinus8 >> 8) & 0xFF);
  header[6] = (byte)((fileSizeMinus8 >> 16) & 0xFF);
  header[7] = (byte)((fileSizeMinus8 >> 24) & 0xFF);
  header[8] = 'W';
  header[9] = 'A';
  header[10] = 'V';
  header[11] = 'E';
  header[12] = 'f';
  header[13] = 'm';
  header[14] = 't';
  header[15] = ' ';
  header[16] = 0x10;  // linear PCM
  header[17] = 0x00;
  header[18] = 0x00;
  header[19] = 0x00;
  header[20] = 0x01;  // linear PCM
  header[21] = 0x00;
  //monoral
  header[22] = (byte)(channels & 0xff);  
  header[23] = 0x00;
   //采样率
  header[24] = (byte) (longSampleRate & 0xff);
  header[25] = (byte) ((longSampleRate >> 8) & 0xff);
  header[26] = (byte) ((longSampleRate >> 16) & 0xff);
  header[27] = (byte) ((longSampleRate >> 24) & 0xff);
  // byte Rate
  header[28] = (byte) (byteRate & 0xff);
  header[29] = (byte) ((byteRate >> 8) & 0xff);
  header[30] = (byte) ((byteRate >> 16) & 0xff);
  header[31] = (byte) ((byteRate >> 24) & 0xff);
  // block align
  header[32] = (byte)(blockAlign & 0xff); 
  header[33] = 0x00;
  // bits per sample
  header[34] = 0x10;  
  header[35] = 0x00;
  header[36] = 'd';
  header[37] = 'a';
  header[38] = 't';
  header[39] = 'a';
  header[40] = (byte)(totalDataLen & 0xFF);
  header[41] = (byte)((totalDataLen >> 8) & 0xFF);
  header[42] = (byte)((totalDataLen >> 16) & 0xFF);
  header[43] = (byte)((totalDataLen >> 24) & 0xFF);
}
