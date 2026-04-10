#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "旺仔小学生";
const char* password = "qwer1234a";

const int LED_PIN = 4;          // 板载LED常见为 GPIO2
const int TOUCH_PIN = T0;       // ESP32 触摸引脚 T0，通常对应 GPIO4

WebServer server(80);

// 系统状态变量
bool isArmed = false;           // 是否布防
bool isAlarmed = false;         // 是否已经触发报警

// 触摸阈值
uint16_t touchThreshold = 30;   // 后面会在 setup() 中自动校准

// LED闪烁控制
unsigned long previousBlinkMillis = 0;
const unsigned long blinkInterval = 100; // 报警闪烁间隔，100ms，高频闪烁
bool ledState = false;

// 生成网页
String makePage() {
  String armState = isArmed ? "已布防" : "未布防";
  String alarmState = isAlarmed ? "报警中" : "正常";
  String ledText = digitalRead(LED_PIN) ? "亮" : "灭";

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 物联网安防报警器</title>
  <style>
    body{
      font-family:Arial, sans-serif;
      text-align:center;
      margin-top:40px;
      background:#f5f5f5;
    }
    .card{
      width:90%;
      max-width:420px;
      margin:auto;
      background:#fff;
      padding:25px;
      border-radius:16px;
      box-shadow:0 4px 12px rgba(0,0,0,0.12);
    }
    h1{
      font-size:24px;
      margin-bottom:20px;
    }
    .status{
      font-size:18px;
      margin:12px 0;
    }
    .label{
      color:#666;
    }
    .value{
      font-weight:bold;
      color:#0078d7;
    }
    button{
      padding:12px 24px;
      font-size:16px;
      margin:10px;
      border:none;
      border-radius:10px;
      cursor:pointer;
    }
    .armBtn{
      background:#28a745;
      color:white;
    }
    .disarmBtn{
      background:#dc3545;
      color:white;
    }
    .tip{
      color:#666;
      font-size:14px;
      margin-top:18px;
      line-height:1.6;
    }
  </style>
</head>
<body>
  <div class="card">
    <h1>物联网安防报警器模拟实验</h1>

    <div class="status"><span class="label">布防状态：</span><span class="value">)rawliteral" + armState + R"rawliteral(</span></div>
    <div class="status"><span class="label">报警状态：</span><span class="value">)rawliteral" + alarmState + R"rawliteral(</span></div>
    <div class="status"><span class="label">LED状态：</span><span class="value">)rawliteral" + ledText + R"rawliteral(</span></div>

    <p>
      <a href="/arm"><button class="armBtn">布防 Arm</button></a>
      <a href="/disarm"><button class="disarmBtn">撤防 Disarm</button></a>
    </p>

    <div class="tip">
      未布防时，触摸引脚无反应。<br>
      布防后，一旦触摸引脚被触发，LED将锁定高频闪烁报警。<br>
      只有点击“撤防”才会停止报警并复位。
    </div>
  </div>
</body>
</html>
)rawliteral";

  return html;
}

// 首页
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// 布防
void handleArm() {
  isArmed = true;
  isAlarmed = false;        // 重新布防时清除旧报警状态
  ledState = false;
  digitalWrite(LED_PIN, LOW);

  server.sendHeader("Location", "/");
  server.send(303);
}

// 撤防
void handleDisarm() {
  isArmed = false;
  isAlarmed = false;
  ledState = false;
  digitalWrite(LED_PIN, LOW);

  server.sendHeader("Location", "/");
  server.send(303);
}

// 读取若干次触摸值，求平均作为基准
uint16_t calibrateTouchBaseline(int samples = 50) {
  uint32_t sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += touchRead(TOUCH_PIN);
    delay(20);
  }
  return sum / samples;
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 触摸基准值自动校准
  Serial.println("开始校准触摸基准值，请勿触摸传感器...");
  uint16_t baseline = calibrateTouchBaseline(50);

  // 经验性阈值：基准值的 70%
  touchThreshold = baseline * 0.7;

  Serial.print("触摸基准值 baseline = ");
  Serial.println(baseline);
  Serial.print("触摸阈值 threshold = ");
  Serial.println(touchThreshold);
  Serial.println("校准完成");

  // 连接 WiFi
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n连接成功");
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());

  // 配置网页路由
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);

  server.begin();
  Serial.println("Web服务器已启动");
}

void loop() {
  server.handleClient();

  // 仅在“已布防且未报警”时检测触摸
  if (isArmed && !isAlarmed) {
    uint16_t touchValue = touchRead(TOUCH_PIN);

    // ESP32 常见判断：触摸后数值下降，低于阈值则判定触发
    if (touchValue < touchThreshold) {
      isAlarmed = true;
      Serial.println("检测到触摸，报警锁定！");
    }
  }

  // 报警状态下 LED 高频闪烁
  if (isAlarmed) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousBlinkMillis >= blinkInterval) {
      previousBlinkMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }
  } else {
    // 非报警状态保持熄灭
    digitalWrite(LED_PIN, LOW);
  }
}