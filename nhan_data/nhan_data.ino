#include <WiFi.h>
#include <HTTPClient.h>

#define RXD2 16
#define TXD2 17

const char* ssid = "Nhan ice";
const char* password = "12345679";

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
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup() {
  Serial.begin(115200);
  
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  
  Serial.print("Đang kết nối WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nĐã kết nối WiFi thành công!");
  Serial.print("Địa chỉ IP của ESP32: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED && Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    data.trim();
    
    // Nếu có dữ liệu
    if (data.length() > 0) {
      String val_Rel_IAQ  = getValue(data, ',', 0);
      String val_IAQ      = getValue(data, ',', 1);
      String val_TVOC     = getValue(data, ',', 2);
      String val_eCO2     = getValue(data, ',', 3);
      String val_EtOH     = getValue(data, ',', 4);
      String val_Temp     = getValue(data, ',', 5);
      String val_Humidity = getValue(data, ',', 6);

      String payload = "{\"Rel_IAQ\":" + val_Rel_IAQ + 
                       ",\"IAQ\":" + val_IAQ + 
                       ",\"TVOC\":" + val_TVOC + 
                       ",\"eCO2\":" + val_eCO2 + 
                       ",\"EtOH\":" + val_EtOH + 
                       ",\"Temp\":" + val_Temp + 
                       ",\"Humidity\":" + val_Humidity + "}";

      Serial.println("Đang gửi đi JSON: " + payload);

      HTTPClient http;
      http.begin("https://unresigned-monopodially-fermina.ngrok-free.dev/update");
      http.addHeader("Content-Type", "application/json"); // Đã chuyển thành json
      
      int httpResponseCode = http.POST(payload);
      
      if (httpResponseCode > 0) {
        Serial.println("Server nhận OK. Trả về mã: " + String(httpResponseCode));
      } else {
        Serial.println("Lỗi gửi HTTP: " + String(httpResponseCode));
      }
      http.end();
    }
  }
  delay(1000);
}