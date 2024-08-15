/*
 * @Description:
            WIFI pull distance testing program for two programs, [GFX_Wifi_STA_Cntract] and [GFX_Wifi_SAP_Cntract]
        WIFI pull distance test, two devices upload the corresponding [GFX_Wifi_STA_Cntract]
    program and [GFX_Wifi_SAP_Cntract] respectively, and the connection status, signal quality,
    and transmission speed of the two devices will be printed on the screen
 * @Author: LILYGO_L
 * @Date: 2024-06-19 17:21:47
 * @LastEditTime: 2024-08-15 12:59:15
 * @License: GPL 3.0
 */

#include <Arduino.h>
#include <WiFi.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"

const char *ssid = "T-Circle-S3";
const char *password = "88888888";

static size_t CycleTime = 0;
static size_t CycleTime_2 = 0;
static uint8_t WiFi_Client_Connection_Timeout_Count = 0;
static size_t Contract_Total_Size = 0;
static size_t Contract_Count_Time = 0;

static bool Device_Status_Flag = false;
static bool Device_Connectio_Success_Initialization_Lock_Flag = true;
static bool Device_Connectio_Fail_Initialization_Lock_Flag = false;

static uint8_t Wifi_Buffer[1024 * 4] = {0};

Arduino_DataBus *bus = new Arduino_ESP32SPIDMA(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */);

Arduino_GFX *gfx = new Arduino_GC9D01N(
    bus, LCD_RST /* RST */, 0 /* rotation */, false /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    ledcAttachPin(LCD_BL, 1);
    ledcSetup(1, 2000, 8);
    ledcWrite(1, 255);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    server.begin();

    Serial.printf("softAPIP address: ");
    Serial.println(WiFi.softAPIP());

    gfx->begin();
    gfx->fillScreen(WHITE);

    gfx->setCursor(45, 15);
    gfx->setTextSize(3);
    gfx->setTextColor(BLACK);
    gfx->printf("Wifi");

    gfx->setCursor(70, 45);
    gfx->setTextSize(2);
    gfx->setTextColor(BLUE);
    gfx->printf("AP");

    gfx->setCursor(5, 70);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);
    gfx->printf("State:");

    gfx->setCursor(45, 70);
    gfx->setTextSize(1);
    gfx->setTextColor(RED);
    gfx->printf("Unconnected Device");

    gfx->setCursor(5, 90);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);
    gfx->printf("Upload");

    gfx->setCursor(10, 100);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);
    gfx->printf("Speed:");

    gfx->setCursor(50, 100);
    gfx->setTextSize(1);
    gfx->setTextColor(RED);
    gfx->printf("none");

    for (int i = 255; i > 0; i--)
    {
        ledcWrite(1, i);
        delay(5);
    }

    // 填充数据包
    memset(Wifi_Buffer, 'A', sizeof(Wifi_Buffer));
}

void loop()
{
    WiFiClient client = server.available();

    if (client)
    {
        Device_Status_Flag = true;

        if (Device_Connectio_Success_Initialization_Lock_Flag == true)
        {
            Device_Connectio_Success_Initialization_Lock_Flag = false;
            Device_Connectio_Fail_Initialization_Lock_Flag = true;
            gfx->fillRect(45, 70, LCD_WIDTH - 45, 10, WHITE);
            gfx->setCursor(45, 70);
            gfx->setTextSize(1);
            gfx->setTextColor(DARKGREEN);
            gfx->printf("Device Connected");
        }

        size_t temp_buff_1 = 0;
        size_t temp_start_time = micros();
        // 发送数据包
        temp_buff_1 = client.write(Wifi_Buffer, sizeof(Wifi_Buffer));
        size_t temp_end_time = micros();

        if (temp_buff_1 > 0)
        {
            Contract_Total_Size += temp_buff_1;
        }
        Contract_Count_Time = Contract_Count_Time + (temp_end_time - temp_start_time);

        if (CycleTime < millis())
        {
            if (Contract_Total_Size > 0 && Contract_Count_Time > 0)
            {
                Serial.printf("Upload Speed: %f KB/s\n", (Contract_Total_Size / 1024.0) / (Contract_Count_Time / 1000.0 / 1000.0));

                gfx->fillRect(50, 100, LCD_WIDTH - 60, 10, WHITE);
                gfx->setCursor(50, 100);
                gfx->setTextSize(1);
                gfx->setTextColor(DARKGREEN);
                gfx->printf("%fKB/s\n", (Contract_Total_Size / 1024.0) / (Contract_Count_Time / 1000.0 / 1000.0));

                WiFi_Client_Connection_Timeout_Count = 0;
                Contract_Total_Size = 0;
                Contract_Count_Time = 0;
            }
            else
            {
                Serial.printf("Upload Speed: 0 KB/s\n");
            }

            CycleTime = millis() + 100;
        }
    }
    else
    {
        client.stop();

        if (CycleTime_2 < millis())
        {
            WiFi_Client_Connection_Timeout_Count++;
            CycleTime_2 = millis() + 1000;
        }

        if (WiFi_Client_Connection_Timeout_Count > 5)
        {
            Device_Status_Flag = false;
        }

        if (Device_Status_Flag == false)
        {
            if (Device_Connectio_Fail_Initialization_Lock_Flag == true)
            {
                Device_Connectio_Fail_Initialization_Lock_Flag = false;
                Device_Connectio_Success_Initialization_Lock_Flag = true;
                gfx->fillRect(45, 70, LCD_WIDTH - 45, 10, WHITE);
                gfx->setCursor(45, 70);
                gfx->setTextSize(1);
                gfx->setTextColor(RED);
                gfx->printf("Unconnected Device");

                gfx->fillRect(50, 100, LCD_WIDTH - 60, 10, WHITE);
                gfx->setCursor(50, 100);
                gfx->setTextSize(1);
                gfx->setTextColor(RED);
                gfx->printf("%fKB/s\n", (Contract_Total_Size / 1024.0) / (Contract_Count_Time / 1000.0 / 1000.0));
            }
        }
    }
}