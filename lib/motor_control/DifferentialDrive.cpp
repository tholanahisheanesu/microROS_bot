#include "DifferentialDrive.h"

DifferentialDrive::DifferentialDrive(MotorDriver& driver, float wheel_separation_m,
                                      float max_linear_velocity_mps)
  : driver_(driver),
    wheel_separation_(wheel_separation_m),
    max_linear_velocity_(max_linear_velocity_mps) {}

void DifferentialDrive::begin() {
  driver_.begin();
}

void DifferentialDrive::setVelocity(float linear_mps, float angular_radps) {
  // Standard differential-drive kinematics, normalized to the driver's [-1, 1] range.
  float left = (linear_mps - angular_radps * wheel_separation_ / 2.0f) / max_linear_velocity_;
  float right = (linear_mps + angular_radps * wheel_separation_ / 2.0f) / max_linear_velocity_;
  driver_.setSpeeds(left, right);
}

void DifferentialDrive::stop() {
  driver_.stop();
}
