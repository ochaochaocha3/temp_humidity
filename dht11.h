#pragma once

#include "Arduino.h"

class Dht11 {
public:
  struct Values {
    int temperature;
    int humidity;

    static Values invalid() {
      return Values{ -99, -1 };
    }

    bool isInvalid() const {
      return temperature == -99 && humidity == -1;
    }
  };

  class InterruptLock {
  public:
    InterruptLock() {
      noInterrupts();
    }

    ~InterruptLock() {
      interrupts();
    }
  };

  Dht11(uint8_t pin, bool debug = false);
  void begin();
  Values readValues();

private:
  uint8_t pin_;
  bool debug_;

  uint8_t bit_;
  uint8_t port_;
  uint32_t max_cycles_;

  uint32_t expectPulse(bool level);
  bool debugPrint(const char* s);
  bool debugPrint(int i);
  bool debugPrintln(const char* s);
  bool debugPrintln(int i);
};
