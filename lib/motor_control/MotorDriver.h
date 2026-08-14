#pragma once

// Common interface for differential-drive motor hardware.
// Speeds are normalized to [-1.0, 1.0]: -1 full reverse, 0 stop, 1 full forward.
class MotorDriver {
public:
  virtual ~MotorDriver() = default;
  virtual void begin() = 0;
  virtual void setSpeeds(float left, float right) = 0;
  void stop() { setSpeeds(0.0f, 0.0f); }
};
