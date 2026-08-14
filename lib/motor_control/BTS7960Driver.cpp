#include "BTS7960Driver.h"

namespace {
constexpr uint8_t kLeftFwdChannel = 0;
constexpr uint8_t kLeftRevChannel = 1;
constexpr uint8_t kRightFwdChannel = 2;
constexpr uint8_t kRightRevChannel = 3;
}  // namespace

BTS7960Driver::BTS7960Driver(int left_rpwm_pin, int left_lpwm_pin, int left_en_pin,
                              int right_rpwm_pin, int right_lpwm_pin, int right_en_pin,
                              uint32_t pwm_freq_hz, uint8_t pwm_resolution_bits)
  : left_rpwm_(left_rpwm_pin), left_lpwm_(left_lpwm_pin), left_en_(left_en_pin),
    right_rpwm_(right_rpwm_pin), right_lpwm_(right_lpwm_pin), right_en_(right_en_pin),
    pwm_freq_(pwm_freq_hz), pwm_res_(pwm_resolution_bits) {}

void BTS7960Driver::begin() {
  pinMode(left_en_, OUTPUT);
  pinMode(right_en_, OUTPUT);
  digitalWrite(left_en_, HIGH);
  digitalWrite(right_en_, HIGH);

  ledcSetup(kLeftFwdChannel, pwm_freq_, pwm_res_);
  ledcAttachPin(left_rpwm_, kLeftFwdChannel);
  ledcSetup(kLeftRevChannel, pwm_freq_, pwm_res_);
  ledcAttachPin(left_lpwm_, kLeftRevChannel);
  ledcSetup(kRightFwdChannel, pwm_freq_, pwm_res_);
  ledcAttachPin(right_rpwm_, kRightFwdChannel);
  ledcSetup(kRightRevChannel, pwm_freq_, pwm_res_);
  ledcAttachPin(right_lpwm_, kRightRevChannel);

  stop();
}

void BTS7960Driver::setSpeeds(float left, float right) {
  writeChannel(left, kLeftFwdChannel, kLeftRevChannel);
  writeChannel(right, kRightFwdChannel, kRightRevChannel);
}

void BTS7960Driver::writeChannel(float speed, uint8_t fwd_channel, uint8_t rev_channel) {
  speed = constrain(speed, -1.0f, 1.0f);
  uint32_t max_duty = (1u << pwm_res_) - 1;
  uint32_t duty = (uint32_t)(fabsf(speed) * max_duty);
  ledcWrite(fwd_channel, speed >= 0.0f ? duty : 0);
  ledcWrite(rev_channel, speed >= 0.0f ? 0 : duty);
}
