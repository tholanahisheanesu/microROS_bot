#include "CmdVelSubscriber.h"
#include <Arduino.h>

CmdVelSubscriber* CmdVelSubscriber::instance_ = nullptr;

CmdVelSubscriber::CmdVelSubscriber(DifferentialDrive& drive, uint32_t timeout_ms)
  : drive_(drive), timeout_ms_(timeout_ms) {
  instance_ = this;  // single robot, single instance
}

rcl_ret_t CmdVelSubscriber::init(rclc_support_t& support, rcl_node_t& node,
                                  rclc_executor_t& executor) {
  rcl_ret_t rc = rclc_subscription_init_default(
    &subscriber_, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist), "cmd_vel");
  if (rc != RCL_RET_OK) return rc;

  const unsigned int watchdog_period_ms = 100;
  rc = rclc_timer_init_default(&watchdog_timer_, &support,
                                RCL_MS_TO_NS(watchdog_period_ms), watchdogTrampoline);
  if (rc != RCL_RET_OK) return rc;

  rc = rclc_executor_add_subscription(&executor, &subscriber_, &msg_, &cmdVelTrampoline,
                                       ON_NEW_DATA);
  if (rc != RCL_RET_OK) return rc;

  rc = rclc_executor_add_timer(&executor, &watchdog_timer_);
  if (rc != RCL_RET_OK) return rc;

  last_cmd_ms_ = millis();
  return RCL_RET_OK;
}

void CmdVelSubscriber::cmdVelTrampoline(const void* msgin) {
  if (instance_) {
    instance_->handleCmdVel(static_cast<const geometry_msgs__msg__Twist*>(msgin));
  }
}

void CmdVelSubscriber::watchdogTrampoline(rcl_timer_t* timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  if (timer != nullptr && instance_) {
    instance_->handleWatchdog();
  }
}

void CmdVelSubscriber::handleCmdVel(const geometry_msgs__msg__Twist* msg) {
  drive_.setVelocity(msg->linear.x, msg->angular.z);
  last_cmd_ms_ = millis();
}

void CmdVelSubscriber::handleWatchdog() {
  if (millis() - last_cmd_ms_ > timeout_ms_) {
    drive_.stop();
  }
}
