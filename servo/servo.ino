#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

#define SS_PIN 5
#define RST_PIN 22

Servo myServo;
MFRC522 mfrc522(SS_PIN, RST_PIN);
bool isCardPresent = false;
bool isServoMoving = false;
unsigned long moveStartTime = 0; // Thời gian bắt đầu quay servo

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  myServo.attach(13);
  myServo.write(90); // Bắt đầu với servo ở vị trí trung tính
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
    if (cardUID == "D34BD12E") 
    {
      if (!isServoMoving) { // Chỉ tiến hành nếu servo không đang di chuyển
       if (!isCardPresent) {
        Serial.println("Thẻ đã được quét. Quay trái.");
        myServo.write(0); // Quay trái
        isServoMoving = true; // Đặt trạng thái đang di chuyển
        moveStartTime = millis(); // Ghi lại thời gian bắt đầu
        isCardPresent = true; // Cập nhật trạng thái thẻ
      } else {
        Serial.println("Thẻ đã được quét lần nữa. Quay phải.");
        myServo.write(180); // Quay phảiQ
        isServoMoving = true; // Đặt trạng thái đang di chuyển
        moveStartTime = millis(); // Ghi lại thời gian bắt đầu
        isCardPresent = false; // Cập nhật trạng thái thẻ
        }
      }
    } else{
      Serial.println("Thẻ không hợp lệ."); // Nếu UID không hợp lệ
    }
      mfrc522.PICC_HaltA(); // Dừng quét thẻ
    }

  // Kiểm tra xem servo đã hoàn thành di chuyển chưa
  if (isServoMoving) {
    if (millis() - moveStartTime >= 3000) { // Nếu đã 3 giây
      myServo.write(90); // Dừng servo
      isServoMoving = false; // Đặt lại trạng thái
    }
  }
}
