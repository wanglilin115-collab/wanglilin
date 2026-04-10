
#include <Arduino.h>

#define LED_A_PIN 2
#define LED_B_PIN 4

int pwm_val = 0;
int step = 2;

void setup() {
  pinMode(LED_A_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
}

void loop() {
  analogWrite(LED_A_PIN, pwm_val);
  analogWrite(LED_B_PIN, 255 - pwm_val);

  pwm_val += step;
  if (pwm_val >= 255 || pwm_val <= 0) {
    step = -step;
  }
  delay(10);
}