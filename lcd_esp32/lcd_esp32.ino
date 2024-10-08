#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int counter = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  lcd.init();                       // Initialize the LCD
  lcd.backlight();                  // Turn on the backlight
  lcd.clear();                      // Clear the LCD screen
  Serial.begin(115200);             // Initialize serial communication
}

void loop() {
  lcd.setCursor(0, 0);               // Set the cursor to the first column and first row
  lcd.print("    INOVATRIX   ");     // Print some text
  Serial.println("hello");
  
  lcd.setCursor(0, 1);               // Set cursor to the second line
  lcd.print(counter);                // Print the counter value
  counter = counter + 1;             // Increment counter
  delay(500);                        // Wait for 1/2 seconds
}
