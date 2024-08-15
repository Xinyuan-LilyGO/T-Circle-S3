/*
 * @Description: 
            Factory testing program
 * @Author: LILYGO_L
 * @Date: 2023-09-06 10:58:19
 * @LastEditTime: 2024-08-15 13:00:30
 * @License: GPL 3.0
 */

#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include "Wire.h"
#include "WiFi.h"
#include <HTTPClient.h>
#include "Arduino_DriveBus_Library.h"
#include "Material_16Bit_160x160px.h"
#include "FastLED.h"
#include "Audio.h"

// 44.1 KHz
#define IIS_SAMPLE_RATE 44100 // 采样速率
#define IIS_DATA_BIT 16       // 数据位数

#define WIFI_SSID "xinyuandianzi"
#define WIFI_PASSWORD "AA15994823428"
// #define WIFI_SSID "LilyGo-AABB"
// #define WIFI_PASSWORD "xinyuandianzi"

#define WIFI_CONNECT_WAIT_MAX (5000)

#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.nist.gov"
#define GMT_OFFSET_SEC 8 * 3600 // Time zone setting function, written as 8 * 3600 in East Eighth Zone (UTC/GMT+8:00)
#define DAY_LIGHT_OFFSET_SEC 0  // Fill in 3600 for daylight saving time, otherwise fill in 0

#define NUM_LEDS 1
#define DATA_PIN APA102_DATA
#define CLOCK_PIN APA102_CLOCK

bool Wifi_Connection_Failure_Flag = false;

static size_t CycleTime = 0;
static size_t CycleTime_2 = 0;

static uint8_t Image_Flag = 0;
static int8_t Volume_Value = 0;
static uint8_t Music_Start_Playing_Count = 0;
static bool Music_Start_Playing_Flag = false;

uint8_t OTG_Mode = 0;
char IIS_Read_Buff[100];

Audio audio;

CRGB leds[NUM_LEDS];

Arduino_DataBus *bus = new Arduino_ESP32SPIDMA(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */);

Arduino_GFX *gfx = new Arduino_GC9D01N(
    bus, LCD_RST /* RST */, 0 /* rotation */, false /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus_0 =
    std::make_shared<Arduino_HWIIS>(I2S_NUM_1, MSM261_BCLK, MSM261_WS, MSM261_DATA);

std::unique_ptr<Arduino_IIS> MSM261(new Arduino_MEMS(IIS_Bus_0));

// std::shared_ptr<Arduino_IIS_DriveBus> IIS_Bus_1 =
//     std::make_shared<Arduino_HWIIS>(I2S_NUM_1, MAX98357A_BCLK, MAX98357A_LRCLK,
//                                     MAX98357A_DATA);

// std::unique_ptr<Arduino_IIS> MAX98357A(new Arduino_Amplifier(IIS_Bus_1, MAX98357A_SD_MODE));

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

void Arduino_IIC_Touch_Interrupt(void);

std::unique_ptr<Arduino_IIC> CST816D(new Arduino_CST816x(IIC_Bus, CST816D_DEVICE_ADDRESS,
                                                         TP_RST, TP_INT, Arduino_IIC_Touch_Interrupt));

void Arduino_IIC_Touch_Interrupt(void)
{
    CST816D->IIC_Interrupt_Flag = true;
}

void Wifi_STA_Test(void)
{
    String text;
    int wifi_num = 0;

    gfx->fillScreen(BLACK);
    gfx->setCursor(0, 0);
    gfx->setTextSize(1);
    gfx->setTextColor(GREEN);

    Serial.println("\nScanning wifi");
    gfx->printf("Scanning wifi\n");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    wifi_num = WiFi.scanNetworks();
    if (wifi_num == 0)
    {
        text = "\nWiFi scan complete !\nNo wifi discovered.\n";
    }
    else
    {
        text = "\nWiFi scan complete !\n";
        text += wifi_num;
        text += " wifi discovered.\n\n";

        for (int i = 0; i < wifi_num; i++)
        {
            text += (i + 1);
            text += ": ";
            text += WiFi.SSID(i);
            text += " (";
            text += WiFi.RSSI(i);
            text += ")";
            text += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " \n" : "*\n";
            delay(10);
        }
    }

    Serial.println(text);
    gfx->println(text);

    delay(3000);
    text.clear();
    gfx->fillScreen(BLACK);
    gfx->setCursor(0, 10);

    text = "Connecting to ";
    Serial.print("Connecting to ");
    gfx->printf("Connecting to\n");
    text += WIFI_SSID;
    text += "\n";

    Serial.print(WIFI_SSID);
    gfx->printf("%s", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    uint32_t last_tick = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        gfx->printf(".");
        text += ".";
        delay(100);

        if (millis() - last_tick > WIFI_CONNECT_WAIT_MAX)
        {
            Wifi_Connection_Failure_Flag = true;
            break;
        }
    }

    if (!Wifi_Connection_Failure_Flag)
    {
        text += "\nThe connection was successful ! \nTakes ";
        Serial.print("\nThe connection was successful ! \nTakes ");
        gfx->printf("\nThe connection was successful ! \nTakes ");
        text += millis() - last_tick;
        Serial.print(millis() - last_tick);
        gfx->print(millis() - last_tick);
        text += " ms\n";
        Serial.println(" ms\n");
        gfx->printf(" ms\n");

        gfx->setTextColor(GREEN);
        gfx->printf("\nWifi test passed!");
    }
    else
    {
        gfx->setTextColor(RED);
        gfx->printf("\nWifi test error!\n");
    }
}

void PrintLocalTime(void)
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 3000))
    {
        Serial.println("Failed to obtain time");
        gfx->setCursor(30, 90);
        gfx->setTextColor(RED);
        gfx->print("Failed to obtain time!");
        return;
    }
    Serial.println("Get time success");
    Serial.println(&timeinfo, "%A,%B %d %Y %H:%M:%S"); // Format Output
    gfx->setCursor(30, 90);
    gfx->setTextColor(ORANGE);
    gfx->print(&timeinfo, " %Y");
    gfx->setCursor(30, 100);
    gfx->print(&timeinfo, "%B %d");
    gfx->setCursor(30, 110);
    gfx->print(&timeinfo, "%H:%M:%S");
}

void GFX_Print_Touch_Info_Loop(int32_t touch_x, int32_t touch_y, int32_t fingers_number)
{
    gfx->fillRect(30, 30, 130, 30, WHITE);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);

    gfx->setCursor(30, 30);
    gfx->printf("ID: %#X ", (int32_t)CST816D->IIC_Device_ID());

    gfx->setCursor(30, 40);
    gfx->printf("Fingers Number:%d ", fingers_number);

    gfx->setCursor(30, 50);
    gfx->printf("Touch X:%d Y:%d ", touch_x, touch_y);
}

void GFX_Print_Voice_Speaker_Info_Loop(int16_t left_channel, int16_t right_channel)
{
    gfx->fillRect(30, 30, 130, 30, WHITE);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);

    gfx->setCursor(30, 30);
    gfx->printf("Voice Data:");

    gfx->setCursor(30, 40);
    gfx->printf("Left Channel:%d ", left_channel);

    gfx->setCursor(30, 50);
    gfx->printf("Right Channel:%d", right_channel);
}

void GFX_Print_Time_Info_Loop()
{
    gfx->fillRoundRect(15, 15, 110, 50, 10, WHITE);

    if (!Wifi_Connection_Failure_Flag)
    {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo, 3000))
        {
            Serial.println("Failed to obtain time");
            gfx->setCursor(20, 20);
            gfx->setTextColor(RED);
            gfx->setTextSize(1);
            gfx->print("Time error");
            return;
        }
        Serial.println("Get time success");
        Serial.println(&timeinfo, "%A,%B %d %Y %H:%M:%S"); // Format Output
        gfx->setCursor(20, 20);
        gfx->setTextColor(ORANGE);
        gfx->setTextSize(1);
        gfx->print(&timeinfo, " %Y");
        gfx->setCursor(20, 30);
        gfx->print(&timeinfo, "%B %d");
        gfx->setCursor(20, 40);
        gfx->print(&timeinfo, "%H:%M:%S");
    }
    else
    {
        gfx->setCursor(20, 20);
        gfx->setTextSize(1);
        gfx->setTextColor(RED);
        gfx->print("Network error");
    }

    gfx->setCursor(20, 50);
    gfx->setTextSize(1);
    gfx->printf("SYS Time:%d", (uint32_t)millis() / 1000);
}

void GFX_Print_1()
{
    gfx->fillRect(15, 100, 60, 30, ORANGE);
    gfx->drawRect(15, 100, 60, 30, PURPLE);
    gfx->fillRect(85, 100, 60, 30, PURPLE);
    gfx->drawRect(85, 100, 60, 30, ORANGE);
    gfx->setTextSize(1);
    gfx->setTextColor(WHITE);
    gfx->setCursor(18, 110);
    gfx->printf("Try Again");
    gfx->setCursor(88, 110);
    gfx->printf("Next Test");
}

void GFX_Print_TEST(String s)
{
    gfx->fillScreen(WHITE);
    gfx->setCursor(45, 30);
    gfx->setTextSize(3);
    gfx->setTextColor(PINK);
    gfx->printf("TEST");

    gfx->setCursor(10, 60);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);
    gfx->print(s);

    gfx->setCursor(70, 90);
    gfx->setTextSize(3);
    gfx->setTextColor(RED);
    gfx->printf("3");
    delay(1000);
    gfx->fillRect(70, 90, 100, 40, WHITE);
    gfx->setCursor(70, 90);
    gfx->printf("2");
    delay(1000);
    gfx->fillRect(70, 90, 100, 40, WHITE);
    gfx->setCursor(70, 90);
    gfx->printf("1");
    delay(1000);
}

void GFX_Print_FINISH()
{
    gfx->setCursor(30, 50);
    gfx->setTextSize(3);
    gfx->setTextColor(ORANGE);
    gfx->printf("FINISH");
}

void GFX_Print_START()
{
    gfx->setCursor(38, 50);
    gfx->setTextSize(3);
    gfx->setTextColor(RED);
    gfx->printf("START");
}

void GFX_Print_Volume()
{
    gfx->setCursor(30, 30);
    gfx->setTextSize(3);
    gfx->setTextColor(ORANGE);
    gfx->printf("Volume");
}

void GFX_Print_Volume_Value()
{
    gfx->fillRect(30, 60, 30, 30, PINK);
    gfx->drawRect(30, 60, 30, 30, PURPLE);
    gfx->fillRect(105, 60, 30, 30, PINK);
    gfx->drawRect(105, 60, 30, 30, PURPLE);
    gfx->setTextSize(1);
    gfx->setTextColor(WHITE);
    gfx->setCursor(42, 70);
    gfx->printf("-");
    gfx->setCursor(117, 70);
    gfx->printf("+");

    gfx->fillRect(75, 70, 30, 30, WHITE);
    gfx->setCursor(75, 70);
    gfx->setTextSize(2);
    gfx->setTextColor(RED);
    gfx->print(audio.getVolume());
}

void GFX_Print_Play_Failed()
{
    gfx->setCursor(20, 30);
    gfx->setTextSize(2);
    gfx->setTextColor(RED);
    gfx->printf("Play failed");
}

void Original_Test_1()
{
    GFX_Print_TEST("1.Touch Test");

    gfx->fillScreen(WHITE);

    int32_t touch_x = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
    int32_t touch_y = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
    uint8_t fingers_number = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

    GFX_Print_Touch_Info_Loop(touch_x, touch_y, fingers_number);

    GFX_Print_1();
}

void Original_Test_2()
{
    GFX_Print_TEST("2.LCD Edge Detection Test");

    gfx->fillScreen(WHITE);
    gfx->drawRect(0, 0, LCD_WIDTH, LCD_HEIGHT, RED);

    GFX_Print_FINISH();

    GFX_Print_1();
}

void Original_Test_3()
{
    GFX_Print_TEST("3.LCD Backlight Test");

    gfx->fillScreen(WHITE);

    GFX_Print_START();

    for (int i = 0; i <= 255; i++)
    {
        ledcWrite(1, i);
        delay(2);
    }
    delay(3000);
    for (int i = 255; i > 0; i--)
    {
        ledcWrite(1, i);
        delay(5);
    }

    delay(1000);

    gfx->fillScreen(WHITE);

    GFX_Print_FINISH();

    GFX_Print_1();
}

void Original_Test_4()
{
    GFX_Print_TEST("4.LCD Color Test");

    gfx->fillScreen(RED);
    delay(3000);
    gfx->fillScreen(GREEN);
    delay(3000);
    gfx->fillScreen(BLUE);
    delay(3000);

    gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_1, LCD_WIDTH, LCD_HEIGHT);
    delay(3000);

    gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_2, LCD_WIDTH, LCD_HEIGHT);
    delay(3000);

    gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_3, LCD_WIDTH, LCD_HEIGHT);
    delay(3000);

    GFX_Print_FINISH();

    GFX_Print_1();
}

void Original_Test_5()
{
    GFX_Print_TEST("5.LED Color Test");

    gfx->fillScreen(WHITE);

    GFX_Print_START();

    leds[0] = CRGB::Red;
    FastLED.show();
    delay(1000);

    leds[0] = CRGB::Green;
    FastLED.show();
    delay(1000);

    leds[0] = CRGB::Blue;
    FastLED.show();
    delay(1000);

    leds[0] = CRGB::White;
    FastLED.show();
    delay(1000);

    leds[0] = CRGB::Black;
    FastLED.show();
    delay(1000);

    gfx->fillScreen(WHITE);

    GFX_Print_FINISH();

    GFX_Print_1();
}

void Original_Test_6()
{
    GFX_Print_TEST("6.MSM261 Test");

    gfx->fillScreen(WHITE);

    gfx->fillRect(30, 30, 130, 30, WHITE);
    gfx->setTextSize(1);
    gfx->setTextColor(BLACK);

    gfx->setCursor(30, 30);
    gfx->printf("MSM261 ON");

    // gfx->setCursor(30, 40);
    // gfx->setTextColor(RED);
    // gfx->printf("Please speak into");

    // gfx->setCursor(30, 50);
    // gfx->printf("the microphone");

    GFX_Print_1();
}

void Original_Test_7()
{
    GFX_Print_TEST("7.WIFI STA Test");

    Wifi_STA_Test();

    delay(2000);

    if (!Wifi_Connection_Failure_Flag)
    {
        // Obtain and set the time from the network time server
        // After successful acquisition, the chip will use the RTC clock to update the holding time
        configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);

        delay(3000);

        PrintLocalTime();
    }
    else
    {
        gfx->setCursor(20, 100);
        gfx->setTextColor(RED);
        gfx->print("Not connected to the network");
    }
    delay(5000);

    gfx->fillScreen(WHITE);

    GFX_Print_FINISH();

    GFX_Print_1();
}

void Original_Test_8()
{
    GFX_Print_TEST("8.Wifi Music Test");

    gfx->fillScreen(BLACK);
    gfx->setCursor(0, 70);
    gfx->setTextSize(1);
    gfx->setTextColor(ORANGE);
    gfx->println("Trying to play music...");

    while (1)
    {
        if (audio.connecttohost("http://music.163.com/song/media/outer/url?id=26122999.mp3") == false)
        {
            Music_Start_Playing_Flag = false;
        }
        else
        {
            Music_Start_Playing_Flag = true;
            break;
        }

        gfx->print(".");

        Music_Start_Playing_Count++;
        if (Music_Start_Playing_Count > 10) // 10秒超时
        {
            Music_Start_Playing_Flag = false;
            break;
        }
        delay(1000);
    }

    Music_Start_Playing_Count = 0;

    while (audio.getAudioCurrentTime() <= 0)
    {
        audio.loop();

        if (millis() > CycleTime)
        {
            Music_Start_Playing_Count++;
            if (Music_Start_Playing_Count > 30) // 30秒后下载超时
            {
                Music_Start_Playing_Flag = false;
                break;
            }
            CycleTime = millis() + 1000;
        }
    }

    if (Music_Start_Playing_Flag == true)
    {
        gfx->fillScreen(WHITE);
        GFX_Print_Volume();
        GFX_Print_Volume_Value();
    }
    else
    {
        gfx->fillScreen(WHITE);
        GFX_Print_Play_Failed();
    }

    GFX_Print_1();
}

void Original_Test_Loop()
{
    Original_Test_1();

    while (1)
    {
        bool temp = false;

        if (CST816D->IIC_Interrupt_Flag == true)
        {
            CST816D->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            GFX_Print_Touch_Info_Loop(touch_x, touch_y, fingers_number);

            if (fingers_number > 0)
            {
                if (touch_x > 15 && touch_x < 75 && touch_y > 100 && touch_y < 130)
                {
                    Original_Test_1();
                }
                if (touch_x > 85 && touch_x < 145 && touch_y > 100 && touch_y < 130)
                {
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_2();

    while (1)
    {
        bool temp = false;

        if (CST816D->IIC_Interrupt_Flag == true)
        {
            CST816D->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 15 && touch_x < 75 && touch_y > 100 && touch_y < 130)
                {
                    Original_Test_2();
                }
                if (touch_x > 85 && touch_x < 145 && touch_y > 100 && touch_y < 130)
                {
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_3();

    while (1)
    {
        bool temp = false;

        if (CST816D->IIC_Interrupt_Flag == true)
        {
            CST816D->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 15 && touch_x < 75 && touch_y > 100 && touch_y < 130)
                {
                    Original_Test_3();
                }
                if (touch_x > 85 && touch_x < 145 && touch_y > 100 && touch_y < 130)
                {
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_4();

    while (1)
    {
        bool temp = false;

        if (CST816D->IIC_Interrupt_Flag == true)
        {
            CST816D->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 15 && touch_x < 75 && touch_y > 100 && touch_y < 130)
                {
                    Original_Test_4();
                }
                if (touch_x > 85 && touch_x < 145 && touch_y > 100 && touch_y < 130)
                {
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_5();

    while (1)
    {
        bool temp = false;

        if (CST816D->IIC_Interrupt_Flag == true)
        {
            CST816D->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 15 && touch_x < 75 && touch_y > 100 && touch_y < 130)
                {
                    Original_Test_5();
                }
                if (touch_x > 85 && touch_x < 145 && touch_y > 100 && touch_y < 130)
                {
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_6();

    while (1)
    {
        bool temp = false;

        if (MSM261->IIS_Read_Data(IIS_Read_Buff, 100) == true)
        {
            // if (MAX98357A->IIS_Write_Data(IIS_Read_Buff, 10) == true)
            // {
            //     // Serial.printf("MAX98357A played successfully\n");
            // 输出左声道数据
            // Serial.printf("Left: %d\n", (int16_t)((int16_t)IIS_Read_Buff[0] | (int16_t)IIS_Read_Buff[1] << 8));
            // 输出右声道数据
            // Serial.printf("Right: %d\n", (int16_t)((int16_t)IIS_Read_Buff[2] | (int16_t)IIS_Read_Buff[3] << 8));

            gfx->setCursor(30, 40);
            gfx->fillRect(50, 40, 100, 30, WHITE);

            gfx->setCursor(30, 40);
            gfx->setTextColor(RED);
            gfx->printf("Left:%d", (int16_t)((int16_t)IIS_Read_Buff[0] | (int16_t)IIS_Read_Buff[1] << 8));

            gfx->setCursor(30, 50);
            gfx->printf("Right:%d", (int16_t)((int16_t)IIS_Read_Buff[2] | (int16_t)IIS_Read_Buff[3] << 8));
            delay(100);
            // }
        }

        if (CST816D->IIC_Interrupt_Flag == true)
        {
            CST816D->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 15 && touch_x < 75 && touch_y > 100 && touch_y < 130)
                {
                    Original_Test_6();
                }
                if (touch_x > 85 && touch_x < 145 && touch_y > 100 && touch_y < 130)
                {
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_7();

    while (1)
    {
        bool temp = false;

        if (CST816D->IIC_Interrupt_Flag == true)
        {
            CST816D->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 15 && touch_x < 75 && touch_y > 100 && touch_y < 130)
                {
                    Original_Test_7();
                }
                if (touch_x > 85 && touch_x < 145 && touch_y > 100 && touch_y < 130)
                {
                    temp = true;
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }

    Original_Test_8();

    while (1)
    {
        bool temp = false;

        if (Music_Start_Playing_Flag == true)
        {
            audio.loop();
        }

        if (CST816D->IIC_Interrupt_Flag == true)
        {
            CST816D->IIC_Interrupt_Flag = false;

            int32_t touch_x = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
            uint8_t fingers_number = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

            if (fingers_number > 0)
            {
                if (touch_x > 15 && touch_x < 75 && touch_y > 100 && touch_y < 130)
                {
                    Original_Test_8();
                }
                if (touch_x > 85 && touch_x < 145 && touch_y > 100 && touch_y < 130)
                {
                    temp = true;
                }

                if (Music_Start_Playing_Flag == true)
                {
                    if (touch_x > 30 && touch_x < 60 && touch_y > 60 && touch_y < 90)
                    {
                        Volume_Value--;
                        if (Volume_Value < 0)
                        {
                            Volume_Value = 0;
                        }
                    }
                    if (touch_x > 103 && touch_x < 133 && touch_y > 60 && touch_y < 90)
                    {
                        Volume_Value++;
                        if (Volume_Value > 21)
                        {
                            Volume_Value = 21;
                        }
                    }

                    audio.setVolume(Volume_Value);
                    GFX_Print_Volume_Value();
                    delay(300);
                }
            }
        }

        if (temp == true)
        {
            break;
        }
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(MAX98357A_SD_MODE, OUTPUT);
    digitalWrite(MAX98357A_SD_MODE, HIGH);

    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH);

    ledcAttachPin(LCD_BL, 1);
    ledcSetup(1, 2000, 8);
    ledcWrite(1, 255);

    if (CST816D->begin() == false)
    {
        Serial.println("CST816D initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("CST816D initialization successfully");
    }

    // 中断模式为检测到触摸时，发出低脉冲
    CST816D->IIC_Write_Device_State(CST816D->Arduino_IIC_Touch::Device::TOUCH_DEVICE_INTERRUPT_MODE,
                                    CST816D->Arduino_IIC_Touch::Device_Mode::TOUCH_DEVICE_INTERRUPT_PERIODIC);

    if (MSM261->begin(Arduino_IIS_DriveBus::Device_Data_Mode::DATA_IN,
                      IIS_SAMPLE_RATE, IIS_DATA_BIT) == false)
    {
        Serial.println("MSM261 initialization fail");
        delay(2000);
    }
    else
    {
        Serial.println("MSM261 initialization successfully");
    }

    // if (MAX98357A->begin(Arduino_IIS_DriveBus::Device_Data_Mode::DATA_OUT,
    //                      IIS_SAMPLE_RATE, IIS_DATA_BIT) == false)
    // {
    //     Serial.println("MAX98357A initialization fail");
    //     delay(2000);
    // }
    // else
    // {
    //     Serial.println("MAX98357A initialization successfully");
    // }

    gfx->begin();
    gfx->fillScreen(BLACK);

    for (int i = 255; i > 0; i--)
    {
        ledcWrite(1, i);
        delay(3);
    }

    FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
    FastLED.setBrightness(20);

    if (audio.setPinout(MAX98357A_BCLK, MAX98357A_LRCLK, MAX98357A_DATA) != true)
    {
        Serial.println("Music driver initialization failed");
        delay(1000);
    }
    else
    {
        Serial.println("Music driver initialization successfully");
    }

    Volume_Value = 3;
    audio.setVolume(Volume_Value); // 0...21,Volume setting

    Original_Test_Loop();

    gfx->fillScreen(PINK);
}

void loop()
{
    if (millis() > CycleTime_2)
    {
        GFX_Print_Time_Info_Loop();
        CycleTime_2 = millis() + 1000;
    }

    if (CST816D->IIC_Interrupt_Flag == true)
    {
        CST816D->IIC_Interrupt_Flag = false;

        uint8_t fingers_number = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

        if (fingers_number > 0)
        {
            delay(300);

            int32_t touch_x = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
            int32_t touch_y = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);

            switch (Image_Flag)
            {
            case 0:
                gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_1, LCD_WIDTH, LCD_HEIGHT); // RGB
                break;
            case 1:
                gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_2, LCD_WIDTH, LCD_HEIGHT); // RGB
                break;
            case 2:
                gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)gImage_3, LCD_WIDTH, LCD_HEIGHT); // RGB
                break;
            case 3:
                gfx->fillScreen(PINK);
                gfx->setCursor(30, 70);
                gfx->setTextColor(YELLOW);
                gfx->setTextSize(2);
                gfx->println("Ciallo1~(L *##*L)^**");
                break;

            default:
                break;
            }

            Image_Flag++;

            if (Image_Flag > 3)
            {
                Image_Flag = 0;
            }

            Serial.printf("[1] point x: %d  point y: %d \r\n", touch_x, touch_y);

            gfx->setTextSize(1);
            gfx->setCursor(touch_x, touch_y);
            gfx->setTextColor(RED);
            gfx->printf("[1] point x: %d  point y: %d \r\n", touch_x, touch_y);
        }
    }
}

// optional
void audio_info(const char *info)
{
    Serial.print("info        ");
    Serial.println(info);
}
void audio_id3data(const char *info)
{ // id3 metadata
    Serial.print("id3data     ");
    Serial.println(info);
}
void audio_eof_mp3(const char *info)
{ // end of file
    Serial.print("eof_mp3     ");
    Serial.println(info);
}
void audio_showstation(const char *info)
{
    Serial.print("station     ");
    Serial.println(info);
}
void audio_showstreamtitle(const char *info)
{
    Serial.print("streamtitle ");
    Serial.println(info);
}
void audio_bitrate(const char *info)
{
    Serial.print("bitrate     ");
    Serial.println(info);
}
void audio_commercial(const char *info)
{ // duration in sec
    Serial.print("commercial  ");
    Serial.println(info);
}
void audio_icyurl(const char *info)
{ // homepage
    Serial.print("icyurl      ");
    Serial.println(info);
}
void audio_lasthost(const char *info)
{ // stream URL played
    Serial.print("lasthost    ");
    Serial.println(info);
}
