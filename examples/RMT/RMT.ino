#include "IRremoteESP8266.h"
#include "IRrecv.h"
#include "IRsend.h"
#include "IRutils.h"
#include "pin_config.h"

IRsend irsend(RMT_TX);
IRrecv irrecv(RMT_RX);

decode_results results;

void setup()
{
    // 初始化串口通信
    Serial.begin(115200);
    Serial.println("Ciallo");

    pinMode(0, INPUT_PULLUP);

    // 启动红外发射器和接收器
    irsend.begin();
    irrecv.enableIRIn();
    Serial.println("IR Receiver is now running");
}

void loop()
{
    // 检查是否接收到红外信号
    if (irrecv.decode(&results))
    {
        // // 打印接收到的红外信号
        // Serial.println("received ir signal:");
        // serialPrintUint64(results.value, HEX);
        // Serial.println("");

        // 解码并打印红外信号的详细信息
        String description = resultToHumanReadableBasic(&results);
        Serial.print(description);

        // 重新启动接收器
        irrecv.resume();
    }

    if (digitalRead(0) == LOW)
    {
        delay(300);

        // 发送数据（这里可以根据需要修改发送的数据）
        Serial.println("send data");

        irrecv.disableIRIn();
        // 例如，发送一个固定的NEC协议的红外信号
        irsend.sendNEC(0x12345678, 32); // 发送一个NEC协议的红外信号

        irrecv.enableIRIn();
    }
}