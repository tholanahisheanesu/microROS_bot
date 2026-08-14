#pragma once
#include <Arduino.h>
#include "MotorDriver.h"

// L298N and similar dual H-bridge boards: one direction-pin pair + one PWM
// enable pin per side. Channel A -> left side, channel B -> right side.
class L298NDriver : public MotorDriver {
public:
  L298NDriver(int ena_pin, int in1_pin, int in2_pin,
              int enb_pin, int in3_pin, int in4_pin,
              uint32_t pwm_freq_hz = 5000, uint8_t pwm_resolution_bits = 8,
              uint8_t left_pwm_channel = 0, uint8_t right_pwm_channel = 1);

  void begin() override;
  void setSpeeds(float left, float right) override;

private:
  void writeChannel(float speed, int pin_fwd, int pin_rev, uint8_t pwm_channel);

  int ena_, in1_, in2_, enb_, in3_, in4_;
  uint32_t pwm_freq_;
  uint8_t pwm_res_;
  uint8_t left_ch_, right_ch_;
};
