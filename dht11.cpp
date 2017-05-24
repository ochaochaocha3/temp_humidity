/*
 * 温湿度センサDHT11の制御
 * https://github.com/adafruit/DHT-sensor-library/blob/15020aa891f81cabb8bad0a9fe66b6c6ef8cf676/DHT.cpp
 * を単純化した
 */

#include "dht11.h"

#include <Arduino.h>

Dht11::Dht11(uint8_t pin, bool debug) :
  pin_(pin),
  debug_(debug),
  bit_(digitalPinToBitMask(pin_)),
  port_(digitalPinToPort(pin_)),
  max_cycles_(microsecondsToClockCycles(200))
{
}

void Dht11::begin() {
  pinMode(pin_, INPUT_PULLUP);
}

Dht11::Values Dht11::readValues() {
  constexpr int Timeout = 200;
  uint8_t data[5] = {};

  // 読み込み前にプルアップする
  digitalWrite(pin_, HIGH);
  delay(250);

  pinMode(pin_, OUTPUT);
  digitalWrite(pin_, LOW);
  delay(30);

  uint32_t cycles[80];
  {
    // ブロック内で割り込み禁止
    InterruptLock lock;

    digitalWrite(pin_, HIGH);
    delayMicroseconds(40);

    pinMode(pin_, INPUT_PULLUP);
    delayMicroseconds(10);

    if (expectPulse(LOW) == 0) {
      debugPrintln("Error,1st wait: LOW");
      return Values::invalid();
    }

    if (expectPulse(HIGH) == 0) {
      debugPrintln("Error,1st wait: HIGH");
      return Values::invalid();
    }

    for (int i = 0; i < 80; i += 2) {
      cycles[i] = expectPulse(LOW);
      cycles[i + 1] = expectPulse(HIGH);
    }
  }

  for (int i = 0; i < 40; ++i) {
    uint32_t lowCycles = cycles[2 * i];
    uint32_t highCycles = cycles[2 * i + 1];

    if (lowCycles == 0 || highCycles == 0) {
      debugPrint("Error,Bit: ");
      debugPrintln(i);

      return Values::invalid();
    }

    data[i / 8] <<= 1;
    if (highCycles > lowCycles) {
      data[i / 8] |= 1;
    }
  }

  uint8_t sum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
  if (data[4] != sum) {
    debugPrintln("Error,Invalid sum");
    return Values::invalid();
  }

  return Values{ static_cast<int>(data[2]), static_cast<int>(data[0]) };
}

uint32_t Dht11::expectPulse(bool level) {
  uint32_t count = 0;
  uint8_t portState = level ? bit_ : 0;

  while ((*portInputRegister(port_) & bit_) == portState) {
    if (count >= max_cycles_) {
      return 0;
    }

    ++count;
  }

  return count;
}

bool Dht11::debugPrint(const char* s) {
  if (!debug_) {
    return false;
  }

  Serial.print(s);

  return true;
}

bool Dht11::debugPrint(int i) {
  if (!debug_) {
    return false;
  }

  Serial.print(i);

  return true;
}

bool Dht11::debugPrintln(const char* s) {
  if (!debug_) {
    return false;
  }

  Serial.println(s);

  return true;
}

bool Dht11::debugPrintln(int i) {
  if (!debug_) {
    return false;
  }

  Serial.println(i);

  return true;
}
