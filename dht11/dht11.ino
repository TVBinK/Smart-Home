#define BLYNK_TEMPLATE_ID "TMPL6ZUB3eOGq"
#define BLYNK_TEMPLATE_NAME "Smart Home"
#define BLYNK_AUTH_TOKEN "5tH-zqq3caTuYxR_Fq675QFDaIY63V1A"

#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>                // Thư viện WiFi cho ESP32

#define DPIN 4        // Pin nối cảm biến DHT (GPIO số)
#define DTYPE DHT11   // Định nghĩa loại cảm biến DHT11 hoặc DHT22

DHT dht(DPIN, DTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Khởi tạo LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Khởi tạo cổng Serial
  Serial.begin(9600);
  
  // Khởi động cảm biến DHT
  dht.begin();
}

void loop() {
  
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
  

  delay(2000);  // Chờ 2 giây trước khi lặp lại
}
