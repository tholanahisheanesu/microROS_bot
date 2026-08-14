#pragma once

// Common interface for wheel encoders. Ticks are a signed cumulative count
// so direction is captured; convert to distance/velocity using your
// encoder's counts-per-revolution and wheel geometry once those are known.
class Encoder {
public:
  virtual ~Encoder() = default;
  virtual void begin() = 0;
  virtual long getTicks() const = 0;
  virtual void reset() = 0;
};
