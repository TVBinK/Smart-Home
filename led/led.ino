#include <WiFi.h>
#include <FirebaseESP32.h>

// Định nghĩa thông tin WiFi và Firebase
#define WIFI_SSID "Phong507"
#define WIFI_PASSWORD "23456789"
#define FIREBASE_HOST "smart-home-52e19-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "Nphho9OXtu9dMyqppI7Jb0aj7Oe8TWYLpXn7yLpE"

// Định nghĩa chân GPIO
const int ledPin = 12;   // Chân GPIO cho LED
const int sensorPin = 14; // Chân GPIO cho TTP223

bool ledState = LOW; // Biến lưu trạng thái LED
FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

void setup() {
  // Khởi động Serial Monitor
  Serial.begin(115200);
  
  // Khởi tạo chân GPIO cho LED và cảm biến
  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT);

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

  // Khởi tạo Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  // Kiểm tra kết nối Firebase
  if (Firebase.beginStream(firebaseData, "/light/isOn")) {
    Serial.println("Kết nối đến Firebase thành công!");
  } else {
    Serial.print("Lỗi kết nối đến Firebase: ");
    Serial.println(firebaseData.errorReason());
  }
}

void loop() {
  // Đọc trạng thái từ TTP223
  int sensorState = digitalRead(sensorPin);
  
  // In ra trạng thái cảm biến
  Serial.println(sensorState);

  static int lastSensorState = LOW; // Lưu trạng thái cảm biến trước đó

  // Kiểm tra nếu cảm biến vừa được kích hoạt
  if (sensorState == HIGH && lastSensorState == LOW) {
    // Đổi trạng thái LED
    ledState = !ledState; // Đảo ngược trạng thái LED
    digitalWrite(ledPin, ledState); // Cập nhật trạng thái LED

    // Cập nhật trạng thái LED lên Firebase
    if (Firebase.setBool(firebaseData, "/light/isOn", !ledState)) {
      Serial.println("Trạng thái LED đã được cập nhật lên Firebase.");
    } else {
      Serial.print("Lỗi cập nhật Firebase: ");
      Serial.println(firebaseData.errorReason());
    }
    
    delay(200); // Thời gian chờ để tránh việc đọc nhiều lần
  }

  // Kiểm tra cập nhật từ Firebase
  if (Firebase.readStream(firebaseData)) {
    if (firebaseData.dataType() == "boolean") {
      bool firebaseIsOn = firebaseData.boolData();
      digitalWrite(ledPin, !firebaseIsOn ? HIGH : LOW); // Cập nhật trạng thái LED từ Firebase
    }
  } else if (firebaseData.dataType() == "undefined") {
    Serial.print("Lỗi đọc Firebase: ");
    Serial.println(firebaseData.errorReason());
  }

  lastSensorState = sensorState; // Cập nhật trạng thái cảm biến trước đó

  // Kiểm tra lại kết nối WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Mất kết nối WiFi! Đang thử kết nối lại...");
    WiFi.reconnect(); // Tự động kết nối lại
  }

  delay(100); // Thời gian chờ để tránh việc đọc quá nhanh
}
