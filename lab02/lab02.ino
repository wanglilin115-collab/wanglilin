#include<Arduino.h>
// 定义板载LED引脚，避免使用"魔法数字"
#define LED_PIN 2
#define LED_PIN_R 7

void setup() {
// 初始化串口通信
Serial.begin(115200);
// 初始化板载LED引脚为输出模式
pinMode(LED_PIN, OUTPUT);
pinMode(LED_PIN_R OUTPUT);
}
void loop() {
Serial.println("Hello ESP32!");
digitalWrite(LED_PIN, HIGH); // 点亮LED
digitalWrite(LED_PIN_R, HIGH);
delay(1000); // 等待1秒
digitalWrite(LED_PIN, LOW); // 熄灭LED
digitalWrite(LED_PIN_R, LOW);
delay(1000); // 等待1秒
}