/*
 * @Description(CN):
 *      该程序用于测试TQT-C6正常工作时的最高功耗
 *
 * @Description(EN):
 *      This program is used to test the maximum power consumption of
 *  TQT-C6 during normal operation.
 *
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-08-18 15:26:31
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2024-03-21 09:44:17
 * @License: GPL 3.0
 */
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define WIFI_SSID "xinyuandianzi"
#define WIFI_PASSWORD "AA15994823428"
// #define WIFI_SSID "LilyGo-AABB"
// #define WIFI_PASSWORD "xinyuandianzi"

#define WIFI_CONNECT_WAIT_MAX 5000

#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.nist.gov"
#define GMT_OFFSET_SEC 8 * 3600 // Time zone setting function, written as 8 * 3600 in East Eighth Zone (UTC/GMT+8:00)
#define DAY_LIGHT_OFFSET_SEC 0  // Fill in 3600 for daylight saving time, otherwise fill in 0

bool Wifi_Connection_State;
static size_t CycleTime = 0;

Arduino_DataBus *bus = new Arduino_ESP32SPIDMA(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */);

Arduino_GFX *gfx = new Arduino_GC9D01N(
    bus, LCD_RST /* RST */, 0 /* rotation */, false /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

// std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
//     std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

// void Arduino_IIC_Touch_Interrupt(void);

// std::unique_ptr<Arduino_IIC> CST816T(new Arduino_CST816x(IIC_Bus, CST816T_DEVICE_ADDRESS,
//                                                          TP_RST, TP_INT, Arduino_IIC_Touch_Interrupt));

// std::unique_ptr<Arduino_IIC> ETA4662(new Arduino_ETA4662(IIC_Bus, ETA4662_DEVICE_ADDRESS,
//                                                          DRIVEBUS_DEFAULT_VALUE, DRIVEBUS_DEFAULT_VALUE));

// void Arduino_IIC_Touch_Interrupt(void)
// {
//     CST816T->IIC_Interrupt_Flag = true;
// }

void WIFI_STA_Test_Loop(void)
{
    gfx->fillScreen(WHITE);
    gfx->setCursor(0, 0);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);

    String temp;
    int wifi_num = 0;
    uint64_t last_tick;

    temp = "Scanning wifi";
    gfx->print(temp);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    wifi_num = WiFi.scanNetworks();
    if (wifi_num == 0)
    {
        temp = "\nWiFi scan complete !\nNo wifi discovered.\n";
    }
    else
    {
        temp = "\nWiFi scan complete !\n";
        temp += wifi_num;
        temp += " wifi discovered.\n\n";

        for (int i = 0; i < wifi_num; i++)
        {
            temp += (i + 1);
            temp += ": ";
            temp += WiFi.SSID(i);
            temp += " (";
            temp += WiFi.RSSI(i);
            temp += ")";
            temp += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " \n" : "*\n";
            delay(10);
        }
    }

    gfx->print(temp);

    temp = "Connecting to ";
    temp += WIFI_SSID;
    temp += "\n";

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    last_tick = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        temp += ".";

        if (millis() - last_tick > WIFI_CONNECT_WAIT_MAX)
        {
            Wifi_Connection_State = false;
            break;
        }
        else
        {
            Wifi_Connection_State = true;
        }
        delay(500); // 这个延时必须要有
    }

    if (Wifi_Connection_State == true)
    {
        temp += "\nThe connection was successful ! \nTakes: ";
        temp += (String)(millis() - last_tick);
        temp += " ms";
    }
    else
    {
        temp += "\nThe connection was fail !";
    }

    gfx->fillScreen(WHITE);
    gfx->setCursor(0, 0);
    gfx->print(temp);

    CycleTime = millis() + 3000;
}

void WIFI_Time(void)
{
    String temp;
    gfx->fillRect(0, 0, 128, 60, WHITE);
    gfx->setCursor(0, 10);

    if (Wifi_Connection_State == true)
    {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo, 10000))
        {
            temp = "Failed to obtain time!";
        }
        else
        {
            temp = "Get time success";
            temp += "\nYear/Month: ";
            temp += (String)(timeinfo.tm_year + 1900);
            temp += "/";
            temp += (String)(timeinfo.tm_mon + 1);
            temp += "/";
            temp += (String)timeinfo.tm_mday;
            temp += "\nTime: ";
            temp += (String)timeinfo.tm_hour;
            temp += " : ";
            temp += (String)timeinfo.tm_min;
            temp += " : ";
            temp += (String)timeinfo.tm_sec;
        }
    }
    else
    {
        temp += "\n\nNot connected to the network";
    }

    gfx->print(temp);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, LOW); // 屏幕
    // ledcAttach(LCD_BL, 2000, 8);
    // ledcWrite(LCD_BL, 0); // brightness 0 - 255

    gfx->begin();
    gfx->fillScreen(WHITE);

    WIFI_STA_Test_Loop();

    delay(2000);

    if (Wifi_Connection_State == true)
    {
        // Obtain and set the time from the network time server
        // After successful acquisition, the chip will use the RTC clock to update the holding time
        configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);

        delay(3000);
    }

    gfx->fillRect(0, 60, 128, 64, WHITE);
    gfx->setCursor(10, 60);
    gfx->setTextSize(1);
    gfx->setTextColor(MAGENTA);
}

void loop()
{
    if (millis() > CycleTime)
    {
        WIFI_STA_Test_Loop();
    }

    if (Wifi_Connection_State == true)
    {
        WIFI_Time();
    }
    else
    {
        gfx->fillRect(0, 0, 128, 60, WHITE);
        gfx->setCursor(0, 10);
        gfx->print("Wifi Connection Fail");
    }

    delay(1000);
}
