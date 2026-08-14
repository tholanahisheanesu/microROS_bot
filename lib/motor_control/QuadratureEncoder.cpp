#include "QuadratureEncoder.h"

QuadratureEncoder::QuadratureEncoder(int pin_a, int pin_b)
  : pin_a_(pin_a), pin_b_(pin_b) {}

void QuadratureEncoder::begin() {
  pinMode(pin_a_, INPUT_PULLUP);
  pinMode(pin_b_, INPUT_PULLUP);
  attachInterruptArg(digitalPinToInterrupt(pin_a_), handleInterrupt, this, CHANGE);
}

long QuadratureEncoder::getTicks() const {
  noInterrupts();
  long t = ticks_;
  interrupts();
  return t;
}

void QuadratureEncoder::reset() {
  noInterrupts();
  ticks_ = 0;
  interrupts();
}

void IRAM_ATTR QuadratureEncoder::handleInterrupt(void* arg) {
  QuadratureEncoder* self = static_cast<QuadratureEncoder*>(arg);
  bool a = digitalRead(self->pin_a_);
  bool b = digitalRead(self->pin_b_);
  self->ticks_ += (a == b) ? 1 : -1;
}
