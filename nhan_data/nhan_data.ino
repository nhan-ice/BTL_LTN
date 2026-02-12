#include <WiFi.h>
#include <HTTPClient.h>
#define RXD2 16
#define TXD2 17

const char* ssid = "PhuocHanh";
const char* password = "tuyetnhung";

void setup() {
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);

    }

void loop() {
  if (WiFi.status() == WL_CONNECTED && Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    data.trim();
    
    int index = data.indexOf(':');
    String cleanData = (index != -1) ? data.substring(index + 1) : data;
    cleanData.trim();

    if (cleanData.length() > 0) {
      HTTPClient http;
      http.begin("http://192.168.1.7:5000/update"); 
      http.addHeader("Content-Type", "text/plain");
      
      int httpResponseCode = http.POST(cleanData); // Đẩy dữ liệu đi
      
      if (httpResponseCode > 0) {
        Serial.println("Đồ thị đã được cập nhật số: " + cleanData);
      }
      http.end();
    }
  }
  delay(1000); 
