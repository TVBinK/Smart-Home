#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

#define SS_PIN 5
#define RST_PIN 22
#define WIFI_SSID "Phong507"
#define WIFI_PASSWORD "23456789"
#define FIREBASE_HOST "https://smart-home-52e19-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "Nphho9OXtu9dMyqppI7Jb0aj7Oe8TWYLpXn7yLpE"

Servo myServo;
MFRC522 mfrc522(SS_PIN, RST_PIN);
FirebaseData firebaseData;

FirebaseConfig config;
FirebaseAuth auth;

bool isCardPresent = false;
bool isServoMoving = false;
unsigned long moveStartTime = 0; // Thời gian bắt đầu quay servo

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  myServo.attach(13); // Gắn servo vào chân 13
  myServo.write(90); // Vị trí trung tính

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
}

void updateDoorStatus(bool isOpened) {
  if (Firebase.setBool(firebaseData, "/door/isOpened", isOpened)) {
    Serial.print("Cập nhật trạng thái cửa thành công: ");
    Serial.println(isOpened);
  } else {
    Serial.println("Lỗi khi cập nhật trạng thái cửa: " + firebaseData.errorReason());
  }
}

void loop() {
  // Kiểm tra xem có thẻ RFID mới hay không
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String cardUID = ""; // Biến lưu trữ UID của thẻ

    // Chuyển đổi UID sang dạng chuỗi
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cardUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    cardUID.toUpperCase(); // Chuyển đổi thành chữ hoa
    
    if (cardUID == "D34BD12E") {
      if (!isServoMoving) { // Chỉ tiến hành nếu servo không đang di chuyển
        if (!isCardPresent) {
          Serial.println("Thẻ đã được quét. Quay trái và mở cửa.");
          myServo.attach(13); // Kết nối lại servo
          myServo.write(0); // Quay trái
          updateDoorStatus(true); // Cập nhật trạng thái mở cửa lên Firebase
          isServoMoving = true;
          moveStartTime = millis(); // Ghi lại thời gian bắt đầu
          isCardPresent = true;
        } else {
          Serial.println("Thẻ đã được quét lần nữa. Quay phải và đóng cửa.");
          myServo.attach(13); // Kết nối lại servo
          myServo.write(180); // Quay phải
          updateDoorStatus(false); // Cập nhật trạng thái đóng cửa lên Firebase
          isServoMoving = true;
          moveStartTime = millis(); // Ghi lại thời gian bắt đầu
          isCardPresent = false;
        }
      }
    } else {
      Serial.println("Thẻ không hợp lệ."); // Nếu UID không hợp lệ
    }
    mfrc522.PICC_HaltA(); // Dừng quét thẻ
  }

  // Kiểm tra xem servo đã hoàn thành di chuyển chưa
  if (isServoMoving) {
    if (millis() - moveStartTime >= 3000) { // Nếu đã 3 giây
      myServo.detach();
      isServoMoving = false; // Đặt lại trạng thái
    }
  }
}
