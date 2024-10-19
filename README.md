<!--
 * @Description: None
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-09-11 16:13:14
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-10-19 10:26:13
 * @License: GPL 3.0
-->
<h1 align = "center">T-Circle-S3</h1>

## **English | [中文](./README_CN.md)**

## Version iteration:
| Version                              | Update date                       |
| :-------------------------------: | :-------------------------------: |
| T-Circle-S3_V1.0                      | 2024-08-15                         |

## PurchaseLink

| Product                     | SOC           |  FLASH  |  PSRAM   | Link                   |
| :------------------------: | :-----------: |:-------: | :---------: | :------------------: |
| T-Circle-S3_V1.0   | ESP32S3R8 |   16M   | 8M (Octal SPI) |   [Not yet sold]()  |

## Directory
- [Describe](#describe)
- [Preview](#preview)
- [Module](#module)
- [QuickStart](#quickstart)
- [PinOverview](#pinoverview)
- [RelatedTests](#RelatedTests)
- [FAQ](#faq)
- [Project](#project)
- [Information](#information)
- [DependentLibraries](#dependentlibraries)

## Describe

T-Circle-S3 is a development board with a 0.75-inch onboard small screen developed based on the ESP32S3, equipped with a speaker, microphone, and three-color LED lights. It has six programmable input/output IO ports on the back, which can be expanded to connect a variety of peripherals.

## Preview

### PCB board

### Rendering

## Module

### 1.MCU

* Chip: ESP32-S3-R8
* PSRAM: 8M (Octal SPI) 
* FLASH: 16M
* For more details, please visit[Espressif ESP32-S3 Datashee](https://www.espressif.com.cn/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)

### 2. Screen

* Size: 0.75-inch LCD round screen
* Resolution: 160x160px
* Screen type: TFT , LCD
* Driver chip: GC9D01N
* Compatibility library: Arduino_GFX
* Bus communication protocol: Standard SPI

### 3. Touch

* Chip: CST816D
* Bus communication protocol: IIC

### 4. Speaker

* Driver Chip: MAX98357A
* Bus Communication Protocol: IIS

### 5. Microphone

* Driver Chip: MSM261S4030H0R
* Bus Communication Protocol: IIS

### 6. LED

* Chip: APA102

## QuickStart

### Examples Support

| Example | Support IDE And Version| Description | Picture |
| ------  | ------  | ------ | ------ | 
| [Animated_Eyes_1](./examples/Animated_Eyes_1) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` |  |  |
| [APA102_Blink](./examples/APA102_Blink) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` |  |  |
| [CST816D](./examples/CST816D) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` |  |  |
| [DMIC_ReadData](./examples/DMIC_ReadData) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` |  |  |
| [DMIC_ReadData](./examples/DMIC_ReadData) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` |  |  |
| [GFX](./examples/GFX) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` |  |  |
| [GFX_CST816D_Image](./examples/GFX_CST816D_Image) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` |  |  |
| [GFX_Wifi_AP_Contract](./examples/GFX_Wifi_AP_Contract) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` |  |  |
| [GFX_Wifi_STA_Contract](./examples/GFX_Wifi_STA_Contract) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` |  |  |
| [IIC_Scan_2](./examples/IIC_Scan_2) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` |  |  |
| [Original_Test](./examples/Original_Test) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` | Original factory test files |  |
| [TFT](./examples/TFT) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` |  |  |
| [Voice_Speaker](./examples/Voice_Speaker) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` |  |  |
| [Voice_Speaker_APA102](./examples/Voice_Speaker_APA102) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` |  |  |
| [Wifi_Music](./examples/Wifi_Music) | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` |  |  |
| [lilygo_s3_apps](https://github.com/Xinyuan-LilyGO/T-Circle-S3/tree/esp-idf-V4.4.8/examples/lilygo_s3_apps) | `[ESP-IDF][esp-idf-V4.4.8]` | This example is a voice control example provided by Grovety. Below is the original link: <br /> [Grovety lilygo_s3_apps](https://github.com/Grovety/lilygo_s3_apps)| <p align="center" width="100%"> <img src="image/1.jpg" alt="example" width="10%"> </p> |

| Firmware | Description | Picture |
| ------  | ------  | ------ |
| [Original_Test_V1.0.0](./firmware/[T-Circle-S3_V1.0][Original_Test]_firmware_V1.0.0.bin) | Original version |  |
| [GFX_Wifi_AP_Contract_V1.0.0](./firmware/[T-Circle-S3_V1.0][GFX_Wifi_AP_Contract]_firmware_V1.0.0) | Original version |  |
| [GFX_Wifi_STA_Contract_V1.0.0](./firmware/[T-Circle-S3_V1.0][GFX_Wifi_STA_Contract]_firmware_V1.0.0) | Original version |  |
| [lilygo_s3_apps_V1.0.0](./firmware/[T-Circle-S3_V1.0]_[lilygo_s3_apps]_firmware_V1.0.0.bin) | Original version |  |

### PlatformIO
1. Install[VisualStudioCode](https://code.visualstudio.com/Download),Choose installation based on your system type.

2. Open the "Extension" section of the Visual Studio Code software sidebar(Alternatively, use "<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>X</kbd>" to open the extension),Search for the "PlatformIO IDE" extension and download it.

3. During the installation of the extension, you can go to GitHub to download the program. You can download the main branch by clicking on the "<> Code" with green text, or you can download the program versions from the "Releases" section in the sidebar.

4. After the installation of the extension is completed, open the Explorer in the sidebar(Alternatively, use "<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>E</kbd>" go open it),Click on "Open Folder," locate the project code you just downloaded (the entire folder), and click "Add." At this point, the project files will be added to your workspace.

5. Open the "platformio.ini" file in the project folder (PlatformIO will automatically open the "platformio.ini" file corresponding to the added folder). Under the "[platformio]" section, uncomment and select the example program you want to burn (it should start with "default_envs = xxx") Then click "<kbd>[√](image/4.png)</kbd>" in the bottom left corner to compile,If the compilation is correct, connect the microcontroller to the computer and click "<kbd>[→](image/5.png)</kbd>" in the bottom left corner to download the program.

### Arduino
1. Install[Arduino](https://www.arduino.cc/en/software),Choose installation based on your system type.

2. Open the "example" directory within the project folder, select the example project folder, and open the file ending with ".ino" to open the Arduino IDE project workspace.

3. Open the "Tools" menu at the top right -> Select "Board" -> "Board Manager." Find or search for "esp32" and download the board files from the author named "Espressif Systems." Then, go back to the "Board" menu and select the development board type under "ESP32 Arduino." The selected development board type should match the one specified in the "platformio.ini" file under the [env] section with the header "board = xxx." If there is no corresponding development board, you may need to manually add the development board from the "board" directory within your project folder.

4. Open menu bar "[File](image/6.png)" -> "[Preferences](image/6.png)" ,Find "[Sketchbook location](image/7.png)"  here,copy and paste all library files and folders from the "libraries" folder in the project directory into the "libraries" folder in this directory.

5. Select the correct settings in the Tools menu, as shown in the table below.

#### ESP32-S3
| Setting                               | Value                                 |
| :-------------------------------: | :-------------------------------: |
| Board                                 | ESP32S3 Dev Module           |
| Upload Speed                     | 921600                               |
| USB Mode                           | Hardware CDC and JTAG     |
| USB CDC On Boot                | Enabled                              |
| USB Firmware MSC On Boot | Disabled                             |
| USB DFU On Boot                | Disabled                             |
| CPU Frequency                   | 240MHz (WiFi)                    |
| Flash Mode                         | QIO 80MHz                         |
| Flash Size                           | 16MB (128Mb)                    |
| Core Debug Level                | None                                 |
| PSRAM                                | OPI PSRAM                         |
| Arduino Runs On                  | Core 1                               |
| Events Run On                     | Core 1                               |           

6. Select the correct port.

7. Click "<kbd>[√](image/8.png)</kbd>" in the upper right corner to compile,If the compilation is correct, connect the microcontroller to the computer,Click "<kbd>[→](image/9.png)</kbd>" in the upper right corner to download.

### firmware download
1. Open the project file "tools" and locate the ESP32 burning tool. Open it.

2. Select the correct burning chip and burning method, then click "OK." As shown in the picture, follow steps 1->2->3->4->5 to burn the program. If the burning is not successful, press and hold the "BOOT-0" button and then download and burn again.

3. Burn the file in the root directory of the project file "[firmware](./firmware/)" file,There is a description of the firmware file version inside, just choose the appropriate version to download.

<p align="center" width="100%">
    <img src="image/10.png" alt="example">
    <img src="image/11.png" alt="example">
</p>


## PinOverview

| LCD screen pins  | ESP32S3 pins|
| :------------------: | :------------------:|
| MOSI         | IO17       |
| DC         | IO16       |
| SCLK         | IO15       |
| CS         | IO13       |
| BL         | IO18       |

| Touch chip pins  | ESP32S3 pins|
| :------------------: | :------------------:|
| INT         | IO12       |
| SDA         | IO11       |
| SCL         | IO14       |

| Microphone pin  | ESP32S3 pins|
| :------------------: | :------------------:|
| BCLK         | IO7       |
| WS         | IO9       |
| DATA         | IO8       |

| Speaker pins  | ESP32S3 pins|
| :------------------: | :------------------:|
| BCLK         | IO5       |
| LRCLK         | IO4       |
| SD_MODE         | IO45       |
| DATA         | IO6       |

| LED pins  | ESP32S3 pins|
| :------------------: | :------------------:|
| CLOCK         | IO39       |
| DATA         | IO38       |

## RelatedTests

## FAQ

* Q. After reading the above tutorials, I still don't know how to build a programming environment. What should I do?
* A. If you still don't understand how to build an environment after reading the above tutorials, you can refer to the [LilyGo-Document](https://github.com/Xinyuan-LilyGO/LilyGo-Document) document instructions to build it.

<br />

* Q. Why does Arduino IDE prompt me to update library files when I open it? Should I update them or not?
* A. Choose not to update library files. Different versions of library files may not be mutually compatible, so it is not recommended to update library files.

<br />

* Q. Why is there no serial data output on the "Uart" interface on my board? Is it defective and unusable?
* A. The default project configuration uses the USB interface as Uart0 serial output for debugging purposes. The "Uart" interface is connected to Uart0, so it won't output any data without configuration.<br />For PlatformIO users, please open the project file "platformio.ini" and modify the option under "build_flags = xxx" from "-D ARDUINO_USB_CDC_ON_BOOT=true" to "-D ARDUINO_USB_CDC_ON_BOOT=false" to enable external "Uart" interface.<br />For Arduino users, open the "Tools" menu and select "USB CDC On Boot: Disabled" to enable the external "Uart" interface.

<br />

* Q. Why is my board continuously failing to download the program?
* A. Please hold down the "BOOT-0" button and try downloading the program again.

## Project
* [T-Circle-S3_V1.0](./project/T-Circle-S3_V1.0_V1.0.pdf)

## Information
* [GC9D01N](./information/GC9D01N.pdf)
* [APA102_2020_LED](./information/APA102_2020_LED.pdf)
* [MSM261S4030H0R](./information/MSM261S4030H0R.pdf)
* [MAX98357AETE+T](./information/MAX98357AETE+T.pdf)

## DependentLibraries
* [Arduino_GFX-1.3.7](https://github.com/moononournation/Arduino_GFX)
* [Arduino_DriveBus-1.1.12](https://github.com/Xk-w/Arduino_DriveBus)
* [ESP32-audioI2S-3.0.6](https://github.com/schreibfaul1/ESP32-audioI2S)
* [FastLED-3.6.0](https://github.com/FastLED/FastLED)
* [TFT_eSPI-2.5.43](https://github.com/Bodmer/TFT_eSPI)

