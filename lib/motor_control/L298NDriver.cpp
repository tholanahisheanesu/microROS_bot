#include "L298NDriver.h"

L298NDriver::L298NDriver(int ena_pin, int in1_pin, int in2_pin,
                          int enb_pin, int in3_pin, int in4_pin,
                          uint32_t pwm_freq_hz, uint8_t pwm_resolution_bits,
                          uint8_t left_pwm_channel, uint8_t right_pwm_channel)
  : ena_(ena_pin), in1_(in1_pin), in2_(in2_pin),
    enb_(enb_pin), in3_(in3_pin), in4_(in4_pin),
    pwm_freq_(pwm_freq_hz), pwm_res_(pwm_resolution_bits),
    left_ch_(left_pwm_channel), right_ch_(right_pwm_channel) {}

void L298NDriver::begin() {
  pinMode(in1_, OUTPUT);
  pinMode(in2_, OUTPUT);
  pinMode(in3_, OUTPUT);
  pinMode(in4_, OUTPUT);

  ledcSetup(left_ch_, pwm_freq_, pwm_res_);
  ledcAttachPin(ena_, left_ch_);
  ledcSetup(right_ch_, pwm_freq_, pwm_res_);
  ledcAttachPin(enb_, right_ch_);

  stop();
}

void L298NDriver::setSpeeds(float left, float right) {
  writeChannel(left, in1_, in2_, left_ch_);
  writeChannel(right, in3_, in4_, right_ch_);
}

void L298NDriver::writeChannel(float speed, int pin_fwd, int pin_rev, uint8_t pwm_channel) {
  speed = constrain(speed, -1.0f, 1.0f);
  digitalWrite(pin_fwd, speed >= 0.0f ? HIGH : LOW);
  digitalWrite(pin_rev, speed >= 0.0f ? LOW : HIGH);
  uint32_t max_duty = (1u << pwm_res_) - 1;
  ledcWrite(pwm_channel, (uint32_t)(fabsf(speed) * max_duty));
}
