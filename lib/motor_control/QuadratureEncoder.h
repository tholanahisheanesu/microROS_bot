#pragma once
#include <Arduino.h>
#include "Encoder.h"

// Standard 2-channel (A/B) quadrature decoding via a pin interrupt on
// channel A, direction read from channel B. Works whether you wire one
// encoder per side or one per motor -- just instantiate one per encoder.
class QuadratureEncoder : public Encoder {
public:
  QuadratureEncoder(int pin_a, int pin_b);

  void begin() override;
  long getTicks() const override;
  void reset() override;

private:
  static void IRAM_ATTR handleInterrupt(void* arg);

  int pin_a_, pin_b_;
  volatile long ticks_ = 0;
};
