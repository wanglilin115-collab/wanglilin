#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "旺仔小学生";
const char* password = "qwer1234a";

const int LED_PIN = 4;          // 板载LED常见为 GPIO2
const int PWM_FREQ = 5000;      // PWM频率 5kHz
const int PWM_RESOLUTION = 8;   // 8位分辨率，对应占空比 0~255

WebServer server(80);

int brightness = 0; // 当前亮度，0~255

String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>实验2：Web无极调光器</title>
  <style>
    body{
      font-family: Arial, sans-serif;
      text-align:center;
      margin-top:50px;
      background:#f5f5f5;
    }
    .card{
      width:90%;
      max-width:420px;
      margin:auto;
      background:white;
      padding:25px;
      border-radius:16px;
      box-shadow:0 4px 12px rgba(0,0,0,0.12);
    }
    h1{
      font-size:24px;
      margin-bottom:20px;
    }
    .value{
      font-size:22px;
      color:#0078d7;
      margin:15px 0;
      font-weight:bold;
    }
    input[type="range"]{
      width:90%;
      height:30px;
    }
    .tip{
      color:#666;
      font-size:14px;
      margin-top:15px;
    }
  </style>
</head>
<body>
  <div class="card">
    <h1>第二部分：Web网页端无极调光器</h1>
    <p>拖动滑动条，实时调节 LED 亮度</p>

    <input type="range" id="slider" min="0" max="255" value=")rawliteral" + String(brightness) + R"rawliteral(">
    <div class="value">当前亮度：<span id="value">)rawliteral" + String(brightness) + R"rawliteral(</span></div>

    <div class="tip">0 = 熄灭，255 = 最亮</div>
  </div>

  <script>
    const slider = document.getElementById("slider");
    const valueText = document.getElementById("value");

    slider.addEventListener("input", function() {
      let val = this.value;
      valueText.textContent = val;

      fetch("/set?value=" + val)
        .then(response => response.text())
        .then(data => console.log("亮度已设置为:", data))
        .catch(error => console.log("请求失败:", error));
    });
  </script>
</body>
</html>
)rawliteral";

  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleSetBrightness() {
  if (server.hasArg("value")) {
    int value = server.arg("value").toInt();
    value = constrain(value, 0, 255);

    brightness = value;
    ledcWrite(LED_PIN, brightness);

    server.send(200, "text/plain", String(brightness));
  } else {
    server.send(400, "text/plain", "Missing value");
  }
}

void setup() {
  Serial.begin(115200);

  // 配置PWM输出
  if (!ledcAttach(LED_PIN, PWM_FREQ, PWM_RESOLUTION)) {
    Serial.println("PWM通道绑定失败！");
    while (1);
  }
  ledcWrite(LED_PIN, brightness);

  // 连接WiFi
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
  server.on("/set", handleSetBrightness);

  server.begin();
  Serial.println("Web服务器已启动");
}

void loop() {
  server.handleClient();
}