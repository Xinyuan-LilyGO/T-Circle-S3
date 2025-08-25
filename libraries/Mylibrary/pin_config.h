/*
 * @Description: None
 * @Author: None
 * @Date: 2023-08-16 14:24:03
 * @LastEditTime: 2025-06-09 11:18:33
 * @License: GPL 3.0
 */
#pragma once

// MAX98357A
#define MAX98357A_BCLK 5
#define MAX98357A_LRCLK 4
#define MAX98357A_DATA 6
#define MAX98357A_SD_MODE 45

// #define T_Circle_S3_V1_0
#define T_Circle_S3_V1_1
#define T_Circle_S3_Infrared_Expansion

#ifdef T_Circle_S3_V1_0
// MSM261
#define MSM261_BCLK 7
#define MSM261_WS 9
#define MSM261_DATA 8

#elif defined T_Circle_S3_V1_1
// MP34DT05TR
#define MP34DT05TR_LRCLK 9
#define MP34DT05TR_DATA 8
#endif

// APA102
#define APA102_DATA 38
#define APA102_CLOCK 39

// H0075Y002-V0
#define LCD_WIDTH 160
#define LCD_HEIGHT 160
#define LCD_MOSI 17
#define LCD_SCLK 15
#define LCD_DC 16
#define LCD_RST -1
#define LCD_CS 13
#define LCD_BL 18

// IIC
#define IIC_SDA 11
#define IIC_SCL 14

#define BOOT_KEY 0

// CST816D
#define TP_SDA IIC_SDA
#define TP_SCL IIC_SCL
#define TP_RST -1
#define TP_INT 12

#ifdef T_Circle_S3_Infrared_Expansion
// IIC
#define IIC_SDA_2 3
#define IIC_SCL_2 2

// ICM20948
#define ICM20948_ADDRESS 0x68
#define ICM20948_SDA IIC_SDA_2
#define ICM20948_SCL IIC_SCL_2
#define ICM20948_INT 42

// RMT
#define RMT_TX 41
#define RMT_RX 40

// Battery
#define BATTERY_ADC_DATA 1

#endif
