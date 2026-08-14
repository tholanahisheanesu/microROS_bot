#pragma once
#include <Arduino.h>
#include <rcl/rcl.h>

// Halts on an unrecoverable rcl error. Anything reaching this point means
// the robot has no working ROS link, so there's nothing safe left to do.
inline void rclErrorLoop() {
  while (true) {
    delay(100);
  }
}

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if (temp_rc != RCL_RET_OK) { rclErrorLoop(); } }
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; (void)temp_rc; }
