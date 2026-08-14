#pragma once
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>
#include "DifferentialDrive.h"

// Subscribes to /cmd_vel and drives the robot; stops the motors if commands
// stop arriving (see timeout_ms) so a lost connection doesn't leave them running.
class CmdVelSubscriber {
public:
  explicit CmdVelSubscriber(DifferentialDrive& drive, uint32_t timeout_ms = 500);

  // Creates the subscription + watchdog timer and registers both with the
  // executor. Caller must have reserved at least 2 executor handle slots.
  rcl_ret_t init(rclc_support_t& support, rcl_node_t& node, rclc_executor_t& executor);

private:
  // rclc callbacks are plain C function pointers with no user-data argument,
  // so we route through a single static instance to reach the real handlers.
  static void cmdVelTrampoline(const void* msgin);
  static void watchdogTrampoline(rcl_timer_t* timer, int64_t last_call_time);
  static CmdVelSubscriber* instance_;

  void handleCmdVel(const geometry_msgs__msg__Twist* msg);
  void handleWatchdog();

  DifferentialDrive& drive_;
  uint32_t timeout_ms_;
  volatile unsigned long last_cmd_ms_ = 0;

  rcl_subscription_t subscriber_{};
  geometry_msgs__msg__Twist msg_{};
  rcl_timer_t watchdog_timer_{};
};
