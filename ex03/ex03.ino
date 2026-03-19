#include<Arduino.h>
// ========== SOS闪灯程序（使用 millis()，不使用 delay()） ==========

// 如果你用的是 Arduino UNO / Nano，LED_BUILTIN 一般就是板载LED（通常是13号引脚）
// 如果你接了外部LED，也可以改成自己的引脚，例如 8、9、10 等
const int ledPin = 5;

// 时间参数（单位：毫秒）
const unsigned long SHORT_ON  = 200;   // 短闪亮灯时间
const unsigned long LONG_ON   = 600;   // 长闪亮灯时间
const unsigned long GAP_OFF   = 200;   // 每次闪烁之间的灭灯间隔
const unsigned long PAUSE_OFF = 1500;  // 一轮SOS结束后的较长停顿

// SOS序列：三短、三长、三短
// 采用“状态 + 持续时间”的方式来写，HIGH表示亮，LOW表示灭
const int stepCount = 18;

const int ledState[stepCount] = {
  HIGH, LOW,   // 短1
  HIGH, LOW,   // 短2
  HIGH, LOW,   // 短3

  HIGH, LOW,   // 长1
  HIGH, LOW,   // 长2
  HIGH, LOW,   // 长3

  HIGH, LOW,   // 短1
  HIGH, LOW,   // 短2
  HIGH, LOW    // 短3 + 末尾长停顿
};

const unsigned long stepDuration[stepCount] = {
  SHORT_ON, GAP_OFF,
  SHORT_ON, GAP_OFF,
  SHORT_ON, GAP_OFF,

  LONG_ON,  GAP_OFF,
  LONG_ON,  GAP_OFF,
  LONG_ON,  GAP_OFF,

  SHORT_ON, GAP_OFF,
  SHORT_ON, GAP_OFF,
  SHORT_ON, PAUSE_OFF
};

int currentStep = 0;              // 当前执行到哪一步
unsigned long previousMillis = 0; // 上一次切换状态的时间

void setup() {
  pinMode(ledPin, OUTPUT);

  // 初始化为序列的第一步
  digitalWrite(ledPin, ledState[currentStep]);
  previousMillis = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  // 到时间了，就切换到下一步
  if (currentMillis - previousMillis >= stepDuration[currentStep]) {
    previousMillis = currentMillis;

    currentStep++;
    if (currentStep >= stepCount) {
      currentStep = 0;  // 播放完一轮后，从头开始
    }

    digitalWrite(ledPin, ledState[currentStep]);
  }
}