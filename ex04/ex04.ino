// ex04: 触摸自锁开关（带防抖）
#include <Arduino.h>

#define TOUCH_PIN 4   // 触摸引脚（T0 对应 GPIO 4）
#define LED_PIN   2   // LED 引脚

bool ledState = false;     // LED 当前状态
bool lastTouchState = false; // 上一次触摸状态
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // 防抖时间（毫秒）

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);
}

void loop() {
  // 读取当前触摸状态（触摸时为 true）
  bool currentTouch = touchRead(TOUCH_PIN) < 40; // 阈值根据实际调整

  // 防抖处理
  if (currentTouch != lastTouchState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // 边缘检测：仅在从未触摸变为触摸的瞬间翻转 LED 状态
    if (currentTouch && !lastTouchState) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
      Serial.print("LED State: ");
      Serial.println(ledState);
    }
  }

  lastTouchState = currentTouch;
}