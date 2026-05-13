#include <WiFi.h>
#include <HTTPClient.h>

#define RXD2 16
#define TXD2 17

const char* ssid = "Nhan ice";
const char* password = "12345679";

String lastAI_Label = "";
String lastAI_Confidence = "";

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return (found > index) ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  Serial.print("Dang ket noi WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nDa ket noi WiFi thanh cong!");
  Serial.print("Dia chi IP cua ESP32: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    delay(1000); // Chỉ chờ nếu mất WiFi
    return;
  }

  // Đọc liên tục nếu có dữ liệu ở bộ đệm UART
  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    data.trim();

    if (data.length() == 0) {
      return;
    }

    // In y hệt như yêu cầu: "UART RX: ..."
    Serial.println("UART RX: " + data);

    // ======================================================
    // 1. NHẬN KẾT QUẢ AI
    // Format: === AI PREDICTS: good (0.9821) ===
    // ======================================================
    if (data.startsWith("=== AI PREDICTS:")) {
      int colonPos = data.indexOf(':');
      int openParen = data.indexOf('(');
      int closeParen = data.indexOf(')');

      if (colonPos > 0 && openParen > colonPos && closeParen > openParen) {
        lastAI_Label = data.substring(colonPos + 1, openParen);
        lastAI_Label.trim();

        lastAI_Confidence = data.substring(openParen + 1, closeParen);
        lastAI_Confidence.trim();
        
        // Không in thêm gì để giữ Terminal sạch đẹp giống hệt trong ảnh tài liệu
      }
      return; // Không gửi HTTP khi nhận được dòng AI
    }

    // ======================================================
    // 2. CHỈ XỬ LÝ DÒNG DỮ LIỆU CẢM BIẾN
    // Format: Rel_IAQ,IAQ,TVOC,eCO2,EtOH,Temp,Humidity
    // ======================================================
    int commaCount = 0;
    for (int i = 0; i < data.length(); i++) {
      if (data.charAt(i) == ',') {
        commaCount++;
      }
    }

    // Nếu không phải chuỗi cảm biến (VD: các dòng "Gom mau AI: 1 / 9"), bỏ qua và không làm gì thêm
    if (commaCount != 6) {
      return;
    }

    String val_Rel_IAQ  = getValue(data, ',', 0);
    String val_IAQ      = getValue(data, ',', 1);
    String val_TVOC     = getValue(data, ',', 2);
    String val_eCO2     = getValue(data, ',', 3);
    String val_EtOH     = getValue(data, ',', 4);
    String val_Temp     = getValue(data, ',', 5);
    String val_Humidity = getValue(data, ',', 6);
    
    // Nếu chưa có kết quả AI thì tự động điền "unknown" và 0.0 theo chuẩn thiết kế
    String aiLabel = (lastAI_Label.length() > 0) ? lastAI_Label : "unknown";
    String aiConf  = (lastAI_Confidence.length() > 0) ? lastAI_Confidence : "0.0";

    // ======================================================
    // 3. TẠO JSON DỮ LIỆU
    // ======================================================
    String payload = 
        "{\"Rel_IAQ\":" + val_Rel_IAQ + 
        ",\"IAQ\":" + val_IAQ + 
        ",\"TVOC\":" + val_TVOC + 
        ",\"eCO2\":" + val_eCO2 + 
        ",\"EtOH\":" + val_EtOH + 
        ",\"Temp\":" + val_Temp + 
        ",\"Humidity\":" + val_Humidity + 
        ",\"AI_Label\":\"" + aiLabel + "\"" + 
        ",\"AI_Confidence\":" + aiConf + "}";

    Serial.println("Dang gui JSON:");
    Serial.println(payload);

    // ======================================================
    // 4. GỬI HTTP POST
    // ======================================================
    HTTPClient http;
    http.begin("https://unresigned-monopodially-fermina.ngrok-free.dev/update");
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.println("Server nhan OK. Ma tra ve: " + String(httpResponseCode));
    } else {
      Serial.println("Loi gui HTTP: " + String(httpResponseCode));
    }

    http.end();
  }
}