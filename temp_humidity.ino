#include <LiquidCrystal.h>
#include "dht11.h"

// LCD
constexpr int E_PIN = 7;
constexpr int RS_PIN = 8;
constexpr int DB4_PIN = 9;
constexpr int DB5_PIN = 10;
constexpr int DB6_PIN = 11;
constexpr int DB7_PIN = 12;

constexpr int DHT11_PIN = 4;

LiquidCrystal lcd(RS_PIN, E_PIN, DB4_PIN, DB5_PIN, DB6_PIN, DB7_PIN);
Dht11 dht11(DHT11_PIN);

void setup() {
  Serial.begin(9600);
  dht11.begin();

  lcd.begin(2, 16);
  lcd.print("Temp[C]: ");
  lcd.setCursor(0, 1);
  lcd.print("Humidity[%]: ");

  delay(2000);
}

void loop() {
  Dht11::Values result = dht11.readValues();
  if (result.isInvalid()) {
    lcd.setCursor(9, 0);
    lcd.print("-------");
    lcd.setCursor(13, 1);
    lcd.print("---");

    Serial.println("-,-");
  } else {
    lcd.setCursor(9, 0);
    lcd.print("       ");

    lcd.setCursor(9, 0);
    lcd.print(result.temperature);

    lcd.setCursor(13, 1);
    lcd.print("   ");

    lcd.setCursor(13, 1);
    lcd.print(result.humidity);

    Serial.print(result.temperature);
    Serial.print(',');
    Serial.println(result.humidity);
  }

  delay(60000);
}
