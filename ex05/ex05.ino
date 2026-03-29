#include <Arduino.h>

// 修正引脚宏定义，避免命名冲突
#define LED_A_PIN 2   // 灯A接GPIO2
#define LED_B_PIN 13  // 灯B接GPIO13

// 全局变量
int pwm_val = 0;     // 占空比（0-255）
int step = 2;        // 渐变步长

void setup() {
  // 初始化引脚为输出
  pinMode(LED_A_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
}

void loop() {
  // 灯A占空比递增，灯B占空比递减（反相关系）
  analogWrite(LED_A_PIN, pwm_val);
  analogWrite(LED_B_PIN, 255 - pwm_val);

  // 更新占空比，到达边界反转方向
  pwm_val += step;
  if (pwm_val >= 255 || pwm_val <= 0) {
    step = -step;
  }
  delay(10);  // 控制渐变平滑度
}