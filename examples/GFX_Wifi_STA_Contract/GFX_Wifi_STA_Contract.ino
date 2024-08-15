/*
 * @Description:
            WIFI pull distance testing program for two programs, [GFX_Wifi_STA_Cntract] and [GFX_Wifi_SAP_Cntract]
        WIFI pull distance test, two devices upload the corresponding [GFX_Wifi_STA_Cntract]
    program and [GFX_Wifi_SAP_Cntract] respectively, and the connection status, signal quality,
    and transmission speed of the two devices will be printed on the screen
 * @Author: LILYGO_L
 * @Date: 2024-06-19 17:21:47
 * @LastEditTime: 2024-08-15 12:05:06
 * @License: GPL 3.0
 */

#include <Arduino.h>
#include <WiFi.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"

const char *ssid = "T-Circle-S3";
const char *password = "88888888";

const char *AP_IP = "192.168.4.1";
static size_t Contract_Total_Size = 0;
static uint8_t WiFi_Client_Connection_Timeout_Count = 0;

static size_t Contract_Count_Time = 0;
static size_t CycleTime = 0;
static size_t CycleTime_2 = 0;

static bool Device_Status_Flag = false;
static bool Device_Connectio_Success_Initialization_Lock_Flag = true;
static bool Device_Connectio_Fail_Initialization_Lock_Flag = false;

Arduino_DataBus *bus = new Arduino_ESP32SPIDMA(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */);

Arduino_GFX *gfx = new Arduino_GC9D01N(
    bus, LCD_RST /* RST */, 0 /* rotation */, false /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

WiFiClient client;

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    ledcAttachPin(LCD_BL, 1);
    ledcSetup(1, 2000, 8);
    ledcWrite(1, 255);

    Serial.printf("WiFi connection successful\n");

    gfx->begin();
    gfx->fillScreen(WHITE);

    gfx->setCursor(45, 15);
    gfx->setTextSize(3);
    gfx->setTextColor(BLACK);
    gfx->printf("Wifi");

    gfx->setCursor(60, 45);
    gfx->setTextSize(2);
    gfx->setTextColor(PURPLE);
    gfx->printf("STA");

    gfx->setCursor(5, 70);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);
    gfx->printf("State:");

    gfx->setCursor(45, 70);
    gfx->setTextSize(1);
    gfx->setTextColor(RED);
    gfx->printf("Unconnected Device");

    gfx->setCursor(5, 80);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);
    gfx->printf("WIFI RSSI:");

    gfx->setCursor(70, 80);
    gfx->setTextSize(1);
    gfx->setTextColor(RED);
    gfx->printf("(none)");

    gfx->setCursor(5, 90);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);
    gfx->printf("Download");

    gfx->setCursor(23, 100);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);
    gfx->printf("Speed:");

    gfx->setCursor(60, 100);
    gfx->setTextSize(1);
    gfx->setTextColor(RED);
    gfx->printf("none");

    for (int i = 255; i > 0; i--)
    {
        ledcWrite(1, i);
        delay(5);
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".\n");

        gfx->fillRect(45, 70, LCD_WIDTH - 45, 10, WHITE);
        gfx->setCursor(45, 70);
        gfx->setTextSize(1);
        gfx->setTextColor(BLUE);
        gfx->printf("Scan Devices...");
    }

    gfx->fillRect(45, 70, LCD_WIDTH - 45, 10, WHITE);
    gfx->setCursor(45, 70);
    gfx->setTextSize(1);
    gfx->setTextColor(BLUE);
    gfx->printf("Unconnected Device");
}

void loop()
{
    if (client.connect(AP_IP, 80, 1000))
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
            gfx->printf("Connected Device");
        }

        while (client.connected())
        {
            if (client.available() > 0)
            {
                size_t temp_buff_1 = 0;
                uint8_t Wifi_Buffer[min(1024 * 4, client.available())] = {0};

                size_t temp_start_time = micros();
                temp_buff_1 = client.read(Wifi_Buffer, sizeof(Wifi_Buffer));
                size_t temp_end_time = micros();

                if (temp_buff_1 > 0)
                {
                    Contract_Total_Size += temp_buff_1;
                }
                Contract_Count_Time = Contract_Count_Time + (temp_end_time - temp_start_time);

                WiFi_Client_Connection_Timeout_Count = 0;
            }

            if (CycleTime < millis())
            {
                Serial.printf("WIFI RSSI: (%d)\n", WiFi.RSSI());

                gfx->fillRect(70, 80, LCD_WIDTH - 60, 10, WHITE);
                gfx->setCursor(70, 80);
                gfx->setTextSize(1);
                gfx->setTextColor(DARKGREEN);
                gfx->printf("(%d)", WiFi.RSSI());

                if (Contract_Total_Size > 0 && Contract_Count_Time > 0)
                {
                    Serial.printf("Download Speed: %f KB/s\n", (Contract_Total_Size / 1024.0) / (Contract_Count_Time / 1000.0 / 1000.0));

                    gfx->fillRect(60, 100, LCD_WIDTH - 60, 10, WHITE);
                    gfx->setCursor(60, 100);
                    gfx->setTextSize(1);
                    gfx->setTextColor(DARKGREEN);
                    gfx->printf("%fKB/s\n", (Contract_Total_Size / 1024.0) / (Contract_Count_Time / 1000.0 / 1000.0));

                    Contract_Total_Size = 0;
                    Contract_Count_Time = 0;
                }
                else
                {
                    Serial.printf("Download Speed: 0 KB/s\n");
                }

                WiFi_Client_Connection_Timeout_Count++;
                CycleTime = millis() + 100;
            }

            if (WiFi_Client_Connection_Timeout_Count > 5)
            {
                WiFi_Client_Connection_Timeout_Count = 0;
                Device_Status_Flag = false;
                break;
            }
        }
        client.stop();
    }
    else
    {
        Serial.printf("Device lost connection\n");

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

                gfx->fillRect(70, 80, LCD_WIDTH - 60, 10, WHITE);
                gfx->setCursor(70, 80);
                gfx->setTextSize(1);
                gfx->setTextColor(RED);
                gfx->printf("(%d)", WiFi.RSSI());

                gfx->fillRect(45, 70, LCD_WIDTH - 45, 10, WHITE);
                gfx->setCursor(45, 70);
                gfx->setTextSize(1);
                gfx->setTextColor(RED);
                gfx->printf("Unconnected Device");

                gfx->fillRect(60, 100, LCD_WIDTH - 60, 10, WHITE);
                gfx->setCursor(60, 100);
                gfx->setTextSize(1);
                gfx->setTextColor(RED);
                gfx->printf("%fKB/s\n", (Contract_Total_Size / 1024.0) / (Contract_Count_Time / 1000.0 / 1000.0));
            }
        }
    }
}
