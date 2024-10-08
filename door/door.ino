#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <ESP32Servo.h>  // Thư viện điều khiển servo cho ESP32
#include <WiFi.h>
#include <FirebaseESP32.h>

// Khai báo địa chỉ I2C cho LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Địa chỉ I2C 0x27, kích thước LCD 16x2

// Khai báo cấu hình cho ma trận 4x4
const byte ROWS = 4;  // Số hàng
const byte COLS = 4;  // Số cột
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {19, 18, 5, 4};  // Kết nối chân hàng (GPIO ESP32)
byte colPins[COLS] = {26, 14, 12, 13};  // Kết nối chân cột (GPIO ESP32)

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);  // Khởi tạo ma trận phím

Servo myServo;  // Tạo đối tượng servo

String inputPassword;    // Chuỗi nhập vào

// Định nghĩa thông tin WiFi và Firebase
#define WIFI_SSID "ae38"
#define WIFI_PASSWORD "19216801"
#define FIREBASE_HOST "smart-home-52e19-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "Nphho9OXtu9dMyqppI7Jb0aj7Oe8TWYLpXn7yLpE"

// Định nghĩa chân GPIO cho relay và cảm biến
const int relayPin = 25;   // Chân GPIO cho relay điều khiển quạt
const int sensorPin = 27;  // Chân GPIO cho TTP223

bool fanState = LOW; // Biến lưu trạng thái quạt (tắt hoặc bật)
FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

void setup() {
  // Khởi tạo LCD, relay, cảm biến, và servo
  lcd.init();
  lcd.backlight();
  myServo.attach(2);  // Gán servo vào chân GPIO 2 (ESP32)
  myServo.write(0);   // Đặt góc ban đầu là 0 độ (cửa đóng)
  
  pinMode(relayPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  digitalWrite(relayPin, LOW);  // Quạt tắt ban đầu
  
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");

  // Kết nối WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Đang kết nối với WiFi...");
  }
  Serial.println("Đã kết nối với WiFi!");

  // Cấu hình Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  // Kiểm tra kết nối Firebase
  if (Firebase.beginStream(firebaseData, "/fan/isOn")) {
    Serial.println("Kết nối đến Firebase thành công!");
  } else {
    Serial.print("Lỗi kết nối đến Firebase: ");
    Serial.println(firebaseData.errorReason());
  }
}

void loop() {
  // Đọc phím từ keypad
  char key = keypad.getKey();

  if (key) {  // Nếu có phím nhấn
    if (key == '#') {  // Nhấn '#' để xác nhận mật khẩu
      if (inputPassword == "2222") {
        myServo.write(90);  // Mở cửa
        lcd.setCursor(0, 1);
        lcd.print("Door Open      ");
      } else if (inputPassword == "3333") {
        myServo.write(0);  // Đóng cửa
        lcd.setCursor(0, 1);
        lcd.print("Door Closed    ");
      } else {
        lcd.setCursor(0, 1);
        lcd.print("Wrong Pass     ");
      }
      inputPassword = "";  // Xóa chuỗi nhập vào
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Password:");
    } else if (key == '*') {  // Nhấn '*' để xóa mật khẩu
      inputPassword = "";
      lcd.setCursor(0, 1);
      lcd.print("Cleared        ");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Password:");
    } else {
      inputPassword += key;  // Thêm ký tự vào chuỗi mật khẩu
      lcd.setCursor(0, 1);
      lcd.print("Pass: " + inputPassword);  // Hiển thị mật khẩu hiện tại
    }
  }

  // Đọc trạng thái từ cảm biến TTP223
  int sensorState = digitalRead(sensorPin);
  static int lastSensorState = LOW;

  if (sensorState == HIGH && lastSensorState == LOW) {
    fanState = !fanState;  // Đổi trạng thái quạt
    digitalWrite(relayPin, fanState);

    // Cập nhật trạng thái lên Firebase
    if (Firebase.setBool(firebaseData, "/fan/isOn", fanState)) {
      Serial.println("Trạng thái quạt đã được cập nhật lên Firebase.");
    } else {
      Serial.print("Lỗi cập nhật Firebase: ");
      Serial.println(firebaseData.errorReason());
    }

    delay(200);
  }

  // Kiểm tra cập nhật từ Firebase
  if (Firebase.readStream(firebaseData)) {
    if (firebaseData.dataType() == "boolean") {
      bool firebaseIsOn = firebaseData.boolData();
      digitalWrite(relayPin, firebaseIsOn ? HIGH : LOW);  // Cập nhật trạng thái quạt
    }
  } else if (firebaseData.dataType() == "undefined") {
    Serial.print("Lỗi đọc Firebase: ");
    Serial.println(firebaseData.errorReason());
  }

  lastSensorState = sensorState;  // Cập nhật trạng thái cảm biến trước đó

  // Kiểm tra lại kết nối WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Mất kết nối WiFi! Đang thử kết nối lại...");
    WiFi.reconnect();
  }

  delay(100);
}
