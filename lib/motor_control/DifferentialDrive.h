#pragma once
#include "MotorDriver.h"

// Converts ROS-style linear/angular velocity (m/s, rad/s) into left/right
// wheel commands for a differential-drive chassis, open-loop (no encoders).
class DifferentialDrive {
public:
  DifferentialDrive(MotorDriver& driver, float wheel_separation_m, float max_linear_velocity_mps);

  void begin();
  void setVelocity(float linear_mps, float angular_radps);
  void stop();

private:
  MotorDriver& driver_;
  float wheel_separation_;
  float max_linear_velocity_;
};
