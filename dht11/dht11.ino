#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define DPIN 4        //Pin to connect DHT sensor (GPIO number)
#define DTYPE DHT11   // Define DHT 11 or DHT22 sensor type

DHT dht(DPIN,DTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {
  lcd.init();                       // Initialize the LCD
  lcd.backlight();                  // Turn on the backlight
  lcd.clear();                      // Clear the LCD screen

  Serial.begin(9600);
  dht.begin();
}

void loop() {
  lcd.setCursor(0, 0); 
  float tc = dht.readTemperature(false);  //Read temperature in C
  float hu = dht.readHumidity();          //Read Humidity

  lcd.print("Temp: ");
  lcd.print(tc);
  lcd.print(" C ");
  lcd.setCursor(0, 1); 
  lcd.print("Hum: ");
  lcd.print(hu);
  lcd.println(" %                 ");

  delay(2000);
}