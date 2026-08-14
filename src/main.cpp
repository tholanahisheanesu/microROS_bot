#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include "MotorDriver.h"
#include "L298NDriver.h"
// #include "BTS7960Driver.h"  // uncomment when switching to the BTS7960 boards
#include "DifferentialDrive.h"
#include "CmdVelSubscriber.h"
#include "RclErrorHandling.h"

#if !defined(MICRO_ROS_TRANSPORT_ARDUINO_SERIAL)
#error This example is only avaliable for Arduino framework with serial transport.
#endif

// --- Motor driver selection ----------------------------------------------
// ACTIVE: L298N driving 4 TT motors, wired 2-in-parallel per side.
#define MOTOR_ENA 25
#define MOTOR_IN1 26
#define MOTOR_IN2 27
#define MOTOR_ENB 14
#define MOTOR_IN3 32
#define MOTOR_IN4 33
L298NDriver motor_driver(MOTOR_ENA, MOTOR_IN1, MOTOR_IN2, MOTOR_ENB, MOTOR_IN3, MOTOR_IN4);

// FUTURE: 2x BTS7960 driving 4 planetary gear motors, 2-in-parallel per side.
// To switch: uncomment the BTS7960Driver.h include above, update the pins
// below for your wiring, comment out the L298NDriver block above, and
// uncomment this one.
// #define MOTOR_LEFT_RPWM  25
// #define MOTOR_LEFT_LPWM  26
// #define MOTOR_LEFT_EN    27
// #define MOTOR_RIGHT_RPWM 14
// #define MOTOR_RIGHT_LPWM 32
// #define MOTOR_RIGHT_EN   33
// BTS7960Driver motor_driver(MOTOR_LEFT_RPWM, MOTOR_LEFT_LPWM, MOTOR_LEFT_EN,
//                            MOTOR_RIGHT_RPWM, MOTOR_RIGHT_LPWM, MOTOR_RIGHT_EN);

// --- Chassis kinematics ----------------------------------------------------
// Placeholders -- tune for your chassis:
// WHEEL_SEPARATION: distance (m) between left/right wheel contact points.
// MAX_LINEAR_VELOCITY: linear speed (m/s) reached at full motor PWM.
#define WHEEL_SEPARATION 0.20f
#define MAX_LINEAR_VELOCITY 0.5f
DifferentialDrive drive_base(motor_driver, WHEEL_SEPARATION, MAX_LINEAR_VELOCITY);

// Motors are force-stopped if /cmd_vel goes silent this long (ms).
#define CMD_VEL_TIMEOUT_MS 500
CmdVelSubscriber cmd_vel_sub(drive_base, CMD_VEL_TIMEOUT_MS);

// --- Wheel encoders (not yet wired in) ------------------------------------
// The planetary gear motors have built-in A/B Hall-effect quadrature
// encoders, matching QuadratureEncoder below. Not yet installed -- update
// pins once wired, call left_encoder.begin()/right_encoder.begin() in
// setup(), and read counts with left_encoder.getTicks()/right_encoder.getTicks().
// #include "QuadratureEncoder.h"
// #define LEFT_ENCODER_A 4
// #define LEFT_ENCODER_B 16
// #define RIGHT_ENCODER_A 17
// #define RIGHT_ENCODER_B 18
// QuadratureEncoder left_encoder(LEFT_ENCODER_A, LEFT_ENCODER_B);
// QuadratureEncoder right_encoder(RIGHT_ENCODER_A, RIGHT_ENCODER_B);

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

void setup() {
  Serial.begin(115200);
  set_microros_serial_transports(Serial);
  delay(2000);

  drive_base.begin();

  allocator = rcl_get_default_allocator();
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  RCCHECK(rclc_node_init_default(&node, "micro_ros_platformio_node", "", &support));

  // 2 handles: cmd_vel subscription + its watchdog timer.
  RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
  RCCHECK(cmd_vel_sub.init(support, node, executor));
}

void loop() {
  delay(100);
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
}
