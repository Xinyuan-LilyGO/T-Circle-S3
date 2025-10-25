<!--
 * @Description: None
 * @Author: LILYGO_L
 * @Date: 2025-01-02 11:24:39
 * @LastEditTime: 2025-10-25 17:23:36
 * @License: GPL 3.0
-->

<h1 align = "center">T-Circle-S3</h1>

<p align="center" width="100%">
    <img src="image/12.jpg" alt="">
</p>

## **English | [中文](./README_CN.md)**

## Version iteration:
| Version                               | Update date                       |Update description|
| :-------------------------------: | :-------------------------------: |:--------------: |
| T-Circle-S3_V1.0                      | 2024-08-15                    |   Original version      |
| T-Circle-S3_V1.1                      | 2025-02-14                    |    Replace microphone model   |
| T-Circle-S3-Infrared-Expansion_V1.0                      | 2025-10-25                    |T-Circle-S3 Infrared Expansion Board    |

## PurchaseLink

| Product                     | SOC           |  FLASH  |  PSRAM   | Link                   |
| :------------------------: | :-----------: |:-------: | :---------: | :------------------: |
| T-Circle-S3_V1.0-V1.1   | ESP32S3R8 |   16M   | 8M (Octal SPI) |  [LILYGO Mall](https://lilygo.cc/products/t-circle-s3?_pos=1&_sid=6fa6d0d3e&_ss=r)  |
| T-Circle-S3-Infrared-Expansion_V1.0   |  |     | |  [Null]()   |

## Directory
- [Describe](#describe)
- [Preview](#preview)
- [Module](#module)
- [SoftwareDeployment](#SoftwareDeployment)
- [PinOverview](#pinoverview)
- [RelatedTests](#RelatedTests)
- [FAQ](#faq)
- [Project](#project)

## Describe

T-Circle-S3 is a development board with a 0.75-inch onboard small screen developed based on the ESP32S3, equipped with a speaker, microphone, and three-color LED lights. It has six programmable input/output IO ports on the back, which can be expanded to connect a variety of peripherals.

T-Circle-S3-Infrared-Expansion is an infrared expansion board for the T-Circle-S3, featuring an onboard infrared sensor, 9-axis sensor, and battery.

## Preview

### Actual Product Image

<p align="center" width="100%">
    <img src="image/12.jpg" alt="">
</p>

---

<p align="center" width="100%">
    <img src="image/13.jpg" alt="">
</p>

---

<p align="center" width="100%">
    <img src="image/14.jpg" alt="">
</p>

## Module

### T-Circle-S3 Section

### 1.MCU

* Chip: ESP32-S3-R8
* PSRAM: 8M (Octal SPI) 
* FLASH: 16M
* Related documentation:
    >[Espressif ESP32-S3 Datasheet](https://www.espressif.com.cn/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)

### 2. Screen

* Size: 0.75-inch LCD round screen
* Resolution: 160x160px
* Screen type: TFT , LCD
* Driver chip: GC9D01N
* Bus communication protocol: Standard SPI
* Related documentation:
    >[GC9D01N](./information/GC9D01N.pdf) <br />
    >[TFT_eSPI-2.5.43](https://github.com/Bodmer/TFT_eSPI)
* Dependent libraries: 
    >[Arduino_GFX-1.3.7](https://github.com/moononournation/Arduino_GFX)

### 3. Touch

* Chip: CST816D
* Bus communication protocol: IIC
* Related documentation:
    >[GC9D01N](./information/GC9D01N.pdf)
* Dependent libraries: 
    >[Arduino_DriveBus-1.1.16]()

### 4. Speaker

* Driver Chip: MAX98357A
* Bus Communication Protocol: IIS
* Related documentation:
    >[MAX98357A](./information/MAX98357AETE+T.pdf)
* Dependent libraries: 
    >[Arduino_DriveBus-1.1.16]() <br />
    >[ESP32-audioI2S-3.0.6](https://github.com/schreibfaul1/ESP32-audioI2S)
    

### 5. Microphone

> #### T-Circle-S3_V1.0 version
> * Chip: MSM261S4030H0R
> * Bus communication protocol: IIS
> * Related documentation: 
>    >[MSM261S4030H0R](./information/MSM261S4030H0R.pdf))
> * Dependent libraries: 
>     >[Arduino_DriveBus-1.1.16]()

> #### T-Circle-S3_V1.1 version
> * Chip: MP34DT05-A
> * Bus communication protocol: PDM
> * Related documentation: 
>    >[MP34DT05-A](./information/mp34dt05-a.pdf)
> * Dependent libraries: 
>    >[Arduino_DriveBus-1.1.16]()


### 6. LED

* Chip: APA102
* Related documentation: 
    >[APA102_2020_LED](./information/APA102_2020_LED.pdf)
* Dependent libraries: 
    >[FastLED-3.6.0](https://github.com/FastLED/FastLED)

### T-Circle-S3-Infrared-Expansion Section

### 1. Infrared Module

* Chip: TSOP75338TR
* Bus Communication Protocol: RMT
* Related Documentation:
    > [TSOP75338TR](./information/TSOP75338TR.pdf)
* Dependent Libraries:
    > [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266)

### 2. IMU

* Chip: ICM20948
* Bus Communication Protocol: IIC
* Related Documentation:
    > [ICM20948](./information/ICM20948.pdf)
* Dependent Libraries:
    > [ICM20948_WE](https://github.com/wollewald/ICM20948_WE)

## QuickStart

### Examples Support

#### T-Circle-S3 examples
| Example | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` | `[ESP-IDF][esp-idf-V4.4.8]`| `[ESP-IDF][esp-idf-V5.3.2]`| Description | Picture |
| ------  | ------  | ------ | ------ | ------ | ------ | 
| [Animated_Eyes_1](./examples/Animated_Eyes_1) |  <p align="center">![alt text][supported] | || |  |
| [APA102_Blink](./examples/APA102_Blink) | <p align="center">![alt text][supported] | | ||  |
| [CST816D](./examples/CST816D) | <p align="center">![alt text][supported] |  |  |
| [DMIC_ReadData](./examples/DMIC_ReadData) | <p align="center">![alt text][supported] | || |  |
| [DMIC_ReadData](./examples/DMIC_ReadData) | <p align="center">![alt text][supported] | || |  |
| [GFX](./examples/GFX) | <p align="center">![alt text][supported] |  |  |
| [GFX_CST816D_Image](./examples/GFX_CST816D_Image) | <p align="center">![alt text][supported] | | ||  |
| [GFX_Wifi_AP_Contract](./examples/GFX_Wifi_AP_Contract) | <p align="center">![alt text][supported] | || |  |
| [GFX_Wifi_STA_Contract](./examples/GFX_Wifi_STA_Contract) | <p align="center">![alt text][supported] | || |  |
| [IIC_Scan_2](./examples/IIC_Scan_2) | <p align="center">![alt text][supported] | | ||  |
| [Original_Test](./examples/Original_Test) | <p align="center">![alt text][supported] ||| Original factory test files |  |
| [TFT](./examples/TFT) | <p align="center">![alt text][supported] | || |  |
| [Voice_Speaker](./examples/Voice_Speaker) | <p align="center">![alt text][supported] |  ||  |  |
| [Voice_Speaker_APA102](./examples/Voice_Speaker_APA102) | <p align="center">![alt text][supported] | ||  |  |
| [Wifi_Music](./examples/Wifi_Music) | <p align="center">![alt text][supported] | ||  |  |
| [lilygo_s3_apps](https://github.com/Xinyuan-LilyGO/T-Circle-S3/tree/esp-idf-V4.4.8/examples/lilygo_s3_apps) || <p align="center">![alt text][supported] | | This example is a voice control example provided by Grovety. Below is the original link: <br /> [Grovety lilygo_s3_apps](https://github.com/Grovety/lilygo_s3_apps)| <p align="center"> <img src="image/1.jpg" alt="example" width="100%"> </p> |
| [XiaoZhi_AI_Chatbot](https://github.com/78/xiaozhi-esp32?tab=readme-ov-file) | || <p align="center">![alt text][supported] | This example is a Xiaozhi AI example, provided by Xiaoxia.| <p align="center"> <img src="image/15.jpg" alt="example" width="100%"> </p> |

#### T-Circle-S3-Infrared-Expansion examples
| Example | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` | `[ESP-IDF][esp-idf-V4.4.8]`| `[ESP-IDF][esp-idf-V5.3.2]`| Description | Picture |
| ------  | ------  | ------ | ------ | ------ | ------ | 
| [RMT](./examples/RMT) |  <p align="center">![alt text][supported] | || |  |
| [ICM20948](./examples/ICM20948) | <p align="center">![alt text][supported] | | ||  |

[supported]: https://img.shields.io/badge/-supported-green "example"

| Firmware | Description | Picture |
| ------  | ------  | ------ |
| [Original_Test(T-Circle-S3_V1.0)](./firmware/[T-Circle-S3_V1.0][Original_Test]_firmware_V1.0.1.bin) | Original factory test files |  |
| [Original_Test(T-Circle-S3_V1.1)](./firmware/（V1.1版本修改麦克风型号）[T-Circle-S3_V1.1][Original_Test]_firmware_202502141426.bin) | Original factory test files |  |
| [GFX_Wifi_AP_Contract](./firmware/[T-Circle-S3_V1.0][GFX_Wifi_AP_Contract]_firmware_V1.0.0) | Original version |  |
| [GFX_Wifi_STA_Contract](./firmware/[T-Circle-S3_V1.0][GFX_Wifi_STA_Contract]_firmware_V1.0.0) | Original version |  |
| [lilygo_s3_apps](./firmware/[T-Circle-S3_V1.0]_[lilygo_s3_apps]_firmware_V1.0.0.bin) | Original version |  |
| [xiaozhi_esp32](./firmware/[T-Circle-S3_V1.0][xiaozhi-esp32_V1.0.1]_firmware_202501240943.bin) | |  |
| [Original_Test(T_Circle_S3_Infrared_Expansion)](./firmware/[T-Circle-S3_V1.0][T_Circle_S3_Infrared_Expansion_V1.0][Original_Test]_firmware_202506091350.bin) | |  |

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
| Partition Scheme                | 16M Flash (3MB APP/9.9MB FATFS) |
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

For pin definitions, please refer to the configuration file: 
<br />

[pin_config.h](./libraries/Mylibrary/pin_config.h)  

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
* [T-Circle-S3_V1.0](./project/T-Circle-S3_V1.0.pdf)
* [T-Circle-S3-Infrared-Expansion_V1.0](./project/T-Circle-S3-Infrared-Expansion_V1.0.pdf)

