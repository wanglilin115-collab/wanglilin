#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "旺仔小学生";
const char* password = "qwer1234a";

const int TOUCH_PIN = 4;   // 经典ESP32中 T0 常对应 GPIO4

WebServer server(80);

// 读取触摸值
uint16_t readTouchValue() {
  return touchRead(TOUCH_PIN);
}

// 生成网页
String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 实时传感器Web仪表盘</title>
  <style>
    body{
      margin:0;
      font-family:Arial, sans-serif;
      background:linear-gradient(135deg, #e8f0ff, #f7fbff);
      height:100vh;
      display:flex;
      justify-content:center;
      align-items:center;
    }
    .card{
      width:90%;
      max-width:500px;
      background:#ffffff;
      border-radius:20px;
      padding:30px 20px;
      text-align:center;
      box-shadow:0 8px 24px rgba(0,0,0,0.12);
    }
    h1{
      margin-top:0;
      font-size:28px;
      color:#222;
    }
    .subtitle{
      color:#666;
      font-size:15px;
      margin-bottom:25px;
      line-height:1.6;
    }
    .value-box{
      margin:20px auto;
      width:220px;
      height:220px;
      border-radius:50%;
      background:#f3f8ff;
      border:8px solid #4a90e2;
      display:flex;
      align-items:center;
      justify-content:center;
      flex-direction:column;
    }
    .value{
      font-size:48px;
      font-weight:bold;
      color:#0078d7;
      line-height:1;
    }
    .unit{
      margin-top:10px;
      font-size:18px;
      color:#666;
    }
    .status{
      margin-top:20px;
      font-size:18px;
      color:#333;
      font-weight:bold;
    }
    .tip{
      margin-top:20px;
      color:#777;
      font-size:14px;
      line-height:1.6;
    }
  </style>
</head>
<body>
  <div class="card">
    <h1>实时传感器 Web 仪表盘</h1>
    <div class="subtitle">
      页面会自动采集并显示 ESP32 触摸传感器实时数值
    </div>

    <div class="value-box">
      <div id="touchValue" class="value">0</div>
      <div class="unit">Touch Value</div>
    </div>

    <div id="touchStatus" class="status">状态：等待数据</div>

    <div class="tip">
      手逐渐靠近触摸引脚时，网页中的数字会实时变化。<br>
      对经典 ESP32，通常是手靠近或触摸后数值变小。
    </div>
  </div>

  <script>
    async function updateTouchValue() {
      try {
        const response = await fetch('/touch');
        const text = await response.text();
        const value = parseInt(text);

        document.getElementById('touchValue').textContent = value;

        if (!isNaN(value)) {
          if (value < 30) {
            document.getElementById('touchStatus').textContent = '状态：已明显触摸/非常接近';
          } else if (value < 50) {
            document.getElementById('touchStatus').textContent = '状态：正在靠近';
          } else {
            document.getElementById('touchStatus').textContent = '状态：未触摸';
          }
        }
      } catch (error) {
        document.getElementById('touchStatus').textContent = '状态：数据获取失败';
      }
    }

    // 页面加载先读一次
    updateTouchValue();

    // 每200ms刷新一次
    setInterval(updateTouchValue, 200);
  </script>
</body>
</html>
)rawliteral";

  return html;
}

// 首页
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// 返回实时触摸值
void handleTouch() {
  uint16_t value = readTouchValue();
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/plain", String(value));
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n连接成功");
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/touch", handleTouch);

  server.begin();
  Serial.println("Web服务器已启动");
}

void loop() {
  server.handleClient();

  // 串口也同步输出，便于调试
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();
    Serial.print("Touch Value = ");
    Serial.println(readTouchValue());
  }
}