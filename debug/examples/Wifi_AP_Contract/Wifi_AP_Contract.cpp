/*
 * @Description: None
 * @version: V1.0.0
 * @Author: None
 * @Date: 2024-06-19 17:21:47
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2024-06-20 15:12:42
 * @License: GPL 3.0
 */

#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "T-Circle-S3";
const char *password = "88888888";

static size_t CycleTime = 0;
static size_t Contract_Total_Size = 0;
static size_t Contract_Count_Time = 0;

static uint8_t Wifi_Buffer[1024 * 4] = {0};

WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    server.begin();

    Serial.printf("softAPIP address: ");
    Serial.println(WiFi.softAPIP());

    // 填充数据包
    memset(Wifi_Buffer, 'A', sizeof(Wifi_Buffer));
}

void loop()
{
    WiFiClient client = server.available();

    if (client)
    {
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

                Contract_Total_Size = 0;
                Contract_Count_Time = 0;
            }
            else
            {
                Serial.printf("Upload Speed: 0 KB/s\n");
            }

            CycleTime = millis() + 1000;
        }
    }
    else
    {
        client.stop();
    }
}