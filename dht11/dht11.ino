#define BLYNK_TEMPLATE_ID "TMPL6ZUB3eOGq"
#define BLYNK_TEMPLATE_NAME "Smart Home"
#define BLYNK_AUTH_TOKEN "5tH-zqq3caTuYxR_Fq675QFDaIY63V1A"

#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>                // Thư viện WiFi cho ESP32
#include <BlynkSimpleEsp32.h>    // Thư viện Blynk cho ESP32

#define DPIN 4        // Pin nối cảm biến DHT (GPIO số)
#define DTYPE DHT11   // Định nghĩa loại cảm biến DHT11 hoặc DHT22

DHT dht(DPIN, DTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Kết nối WiFi và Blynk
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Phong507";  // Thay bằng tên WiFi của bạn
char pass[] = "23456789";  // Thay bằng mật khẩu WiFi của bạn

void setup() {
  // Khởi tạo LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Khởi tạo cổng Serial
  Serial.begin(9600);
  
  // Khởi động Blynk
  Blynk.begin(auth, ssid, pass);
  
  // Khởi động cảm biến DHT
  dht.begin();
}

void loop() {
  Blynk.run();  // Điều khiển kết nối với Blynk
  
  // Đọc nhiệt độ và độ ẩm
  double tc = dht.readTemperature(false);  // Đọc nhiệt độ (C)
  double hu = dht.readHumidity();          // Đọc độ ẩm
  
  // Hiển thị dữ liệu lên LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(tc);
  lcd.print(" C ");
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(hu);
  lcd.print(" %                 ");
  
  // Gửi dữ liệu lên Blynk
  Blynk.virtualWrite(V0, tc);  // Gửi nhiệt độ lên Virtual Pin V0
  Blynk.virtualWrite(V1, hu);  // Gửi độ ẩm lên Virtual Pin V1

  delay(2000);  // Chờ 2 giây trước khi lặp lại
}
