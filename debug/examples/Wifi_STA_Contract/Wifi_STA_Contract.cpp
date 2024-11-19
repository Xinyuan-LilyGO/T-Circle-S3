/*
 * @Description: None
 * @version: V1.0.0
 * @Author: None
 * @Date: 2024-06-19 17:30:04
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2024-06-20 15:07:57
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "T-Circle-S3";
const char *password = "88888888";

const char *AP_IP = "192.168.4.1";
static size_t Contract_Total_Size = 0;

static size_t Contract_Start_Time = 0;
static size_t Contract_End_Time = 0;
static size_t Contract_Count_Time = 0;
static size_t CycleTime = 0;

static uint8_t WiFi_Client_Connection_Timeout_Count = 0;

WiFiClient client;

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".\n");
    }

    Serial.printf("WiFi connection successful\n");
}

void loop()
{
    if (client.connect(AP_IP, 80, 1000))
    {
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
                if (Contract_Total_Size > 0 && Contract_Count_Time > 0)
                {
                    Serial.printf("Download Speed: %f KB/s\n", (Contract_Total_Size / 1024.0) / (Contract_Count_Time / 1000.0 / 1000.0));

                    Contract_Total_Size = 0;
                    Contract_Count_Time = 0;
                }
                else
                {
                    Serial.printf("Download Speed: 0 KB/s\n");
                }

                WiFi_Client_Connection_Timeout_Count++;
                CycleTime = millis() + 1000;
            }

            if (WiFi_Client_Connection_Timeout_Count > 6)
            {
                WiFi_Client_Connection_Timeout_Count = 0;
                break;
            }
        }
        client.stop();
    }
    else
    {
        Serial.printf("Device lost connection\n");
    }
}