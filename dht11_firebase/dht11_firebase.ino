#include <WiFi.h>
#include <FirebaseESP32.h>
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DPIN 4        // Pin để kết nối cảm biến DHT (GPIO)
#define DTYPE DHT11   // Định nghĩa loại cảm biến DHT (DHT11 hoặc DHT22)

// Thông tin kết nối Wi-Fi
#define WIFI_SSID "ae38"
#define WIFI_PASSWORD "19216801"

// Thông tin cấu hình Firebase
#define FIREBASE_HOST "smart-home-52e19-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "Nphho9OXtu9dMyqppI7Jb0aj7Oe8TWYLpXn7yLpE"

DHT dht(DPIN, DTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Kiểm tra lại địa chỉ I2C nếu không phải 0x27

// Cấu hình Firebase
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  // Khởi tạo LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  Serial.begin(115200);

  // Khởi tạo cảm biến DHT
  dht.begin();

  // Kết nối Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Đang kết nối WiFi...");
  }
  Serial.println("Đã kết nối WiFi");

  // Cấu hình Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Bắt đầu kết nối Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true); // Tự động kết nối lại Wi-Fi nếu bị mất kết nối

  // Kiểm tra trạng thái Firebase
  if (!Firebase.ready()) {
    Serial.println("Firebase không sẵn sàng");
  } else {
    Serial.println("Firebase đã sẵn sàng");
  }
}

void loop() {
  lcd.setCursor(0, 0); 
  float tc = dht.readTemperature(false);  // Đọc nhiệt độ bằng DHT (đơn vị Celsius)
  float hu = dht.readHumidity();          // Đọc độ ẩm bằng DHT

  // In ra Serial để kiểm tra dữ liệu
  Serial.print("Temperature: ");
  Serial.println(tc);
  Serial.print("Humidity: ");
  Serial.println(hu);

  // Hiển thị dữ liệu trên màn hình LCD
  lcd.print("Temp: ");
  lcd.print(tc);
  lcd.print(" C ");
  lcd.setCursor(0, 1); 
  lcd.print("Hum: ");
  lcd.print(hu);
  lcd.println(" %                ");

  // Gửi dữ liệu lên Firebase
  if (Firebase.ready()) {
    Firebase.setFloat(firebaseData, "/environment/temperature", tc);
    Firebase.setFloat(firebaseData, "/environment/humidity", hu);
  }
  delay(2000); // Chờ 2 giây trước khi thực hiện đọc lại
}
