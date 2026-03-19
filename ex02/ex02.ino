#include<Arduino.h>
const int ledPin = 5;       // 定义LED引脚（Arduino板载LED为13脚）
unsigned long previousMillis = 0;  // 记录上一次LED状态切换的时间
const long interval = 1000;  // 闪烁间隔（1000ms=1秒）
int ledState = LOW;          // 初始LED状态

void setup() {
  pinMode(ledPin, OUTPUT);   // 设置引脚为输出模式
}

void loop() {
  unsigned long currentMillis = millis();  // 获取当前系统运行毫秒数
  
  // 当时间间隔达到1秒时，切换LED状态
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // 更新上一次切换时间
    ledState = !ledState;            // 反转LED状态
    digitalWrite(ledPin, ledState);  // 写入新状态
  }
}