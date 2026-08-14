#pragma once
#include <Arduino.h>
#include "MotorDriver.h"

// BTS7960 / IBT-2 modules: one RPWM/LPWM pin pair per side (no separate
// direction pin -- speed magnitude drives whichever of RPWM/LPWM matches
// the requested direction, the other pin stays at 0). EN pins are held
// high in begin(); tie R_EN/L_EN together in hardware, or wire both to
// the same GPIO passed in here.
class BTS7960Driver : public MotorDriver {
public:
  BTS7960Driver(int left_rpwm_pin, int left_lpwm_pin, int left_en_pin,
                int right_rpwm_pin, int right_lpwm_pin, int right_en_pin,
                uint32_t pwm_freq_hz = 5000, uint8_t pwm_resolution_bits = 8);

  void begin() override;
  void setSpeeds(float left, float right) override;

private:
  void writeChannel(float speed, uint8_t fwd_channel, uint8_t rev_channel);

  int left_rpwm_, left_lpwm_, left_en_;
  int right_rpwm_, right_lpwm_, right_en_;
  uint32_t pwm_freq_;
  uint8_t pwm_res_;
};
