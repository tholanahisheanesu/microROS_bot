# microROS_bot

ESP32 firmware for a differential-drive robot base, built on [micro-ROS](https://micro.ros.org/) over serial. Subscribes to `/cmd_vel` (`geometry_msgs/msg/Twist`) and drives the motors accordingly. Requires a micro-ROS agent running on a companion PC to bridge the ESP32 onto the ROS 2 graph.

## Hardware

| Stage | Driver | Motors |
|---|---|---|
| Current bring-up | L298N | 4x TT motors, 2 in parallel per side |
| Final | 2x BTS7960 43A | 4x 12V 560RPM 22mm planetary gear motors w/ built-in quadrature encoders, 2 in parallel per side |

Board: ESP32 Dev Module. ROS 2 distro: **Jazzy** (set via `board_microros_distro` in [platformio.ini](platformio.ini)).

## Repo layout

```
src/main.cpp                  Entry point: picks hardware, wires everything together
lib/motor_control/
  MotorDriver.h                Interface: begin(), setSpeeds(left, right) in [-1,1], stop()
  L298NDriver.h/.cpp            Active driver
  BTS7960Driver.h/.cpp          Ready, commented out in main.cpp until the new hardware is wired in
  DifferentialDrive.h/.cpp      linear/angular velocity -> left/right wheel speeds
  Encoder.h                     Interface: begin(), getTicks(), reset()
  QuadratureEncoder.h/.cpp      A/B Hall-effect decoder, matches the new gear motors' built-in encoders
lib/ros_comm/
  CmdVelSubscriber.h/.cpp       Owns the /cmd_vel subscription + a watchdog that stops motors if commands go stale
  RclErrorHandling.h            RCCHECK/RCSOFTCHECK macros
```

Swapping hardware (e.g. L298N -> BTS7960, or activating encoders) only means editing the clearly marked comment blocks at the top of `src/main.cpp` — no other files need to change.

## 1. Flashing the ESP32

**Prerequisites:** [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/index.html) or VS Code + the PlatformIO extension.

```bash
pio run                # build
pio run -t upload      # flash
pio device monitor      # optional: view serial output
```

**Linux serial permissions:** if upload fails with `Permission denied: '/dev/ttyUSB0'`, your user isn't in the `dialout` group:
```bash
sudo usermod -aG dialout $USER
```
Then **log out and back in** (group membership loads at login, not per-terminal).

### Wiring — L298N (active by default)

| Signal | GPIO |
|---|---|
| ENA (left PWM) | 25 |
| IN1 / IN2 (left dir) | 26 / 27 |
| ENB (right PWM) | 14 |
| IN3 / IN4 (right dir) | 32 / 33 |

### Wiring — BTS7960 (once installed, uncomment in `src/main.cpp`)

| Signal | GPIO |
|---|---|
| Left RPWM / LPWM / EN | 25 / 26 / 27 |
| Right RPWM / LPWM / EN | 14 / 32 / 33 |

Update these to match your actual wiring before uploading.

### Tuning

Two placeholders at the top of `src/main.cpp`, needed since there's no closed-loop feedback yet:
- `WHEEL_SEPARATION` — distance (m) between left/right wheel contact points
- `MAX_LINEAR_VELOCITY` — real linear speed (m/s) at full motor PWM

## 2. micro-ROS agent (on the PC / companion computer)

The agent is a separate ROS 2 workspace, not part of this repo. Requires ROS 2 Jazzy already installed.

```bash
mkdir -p ~/microros_ws/src
cd ~/microros_ws
git clone -b $ROS_DISTRO https://github.com/micro-ROS/micro_ros_setup.git src/micro_ros_setup
rosdep update && rosdep install --from-paths src --ignore-src -y
colcon build
source install/setup.bash

ros2 run micro_ros_setup create_agent_ws.sh   # pulls in the agent packages
colcon build
source install/setup.bash
```

Run the agent (matches the firmware's `Serial.begin(115200)`):
```bash
ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0 -b 115200
```

> If you see `Package 'micro_ros_agent' not found`, you forgot to `source install/setup.bash` in that terminal.

**After the agent is running, reset the ESP32** (EN/RST button or power-cycle) — the handshake starts from the board's boot sequence, so the agent must already be listening. You should see a `session established` line appear in the agent's terminal.

Verify the connection from another terminal:
```bash
ros2 topic list
# should include /cmd_vel among the standard topics
```

## 3. Driving the robot

```bash
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{linear: {x: 0.1}, angular: {z: 0.0}}" -r 10
```
Or drive it with `teleop_twist_keyboard`:
```bash
ros2 run teleop_twist_keyboard teleop_twist_keyboard
```

If `/cmd_vel` stops publishing (Ctrl-C, lost connection, etc.), the firmware force-stops the motors after 500ms (`CMD_VEL_TIMEOUT_MS` in `src/main.cpp`) rather than continuing the last command.

## Troubleshooting

| Symptom | Fix |
|---|---|
| `Permission denied: '/dev/ttyUSB0'` on upload or agent start | Add your user to `dialout` (see above) and log out/in. One-off fix without logging out: `sudo chmod 666 /dev/ttyUSB0`. |
| `Package 'micro_ros_agent' not found` | `source install/setup.bash` in the agent workspace. |
| Agent runs but `ros2 topic list` never shows the robot's topics | Reset/power-cycle the ESP32 after the agent has started. |
| Firmware build fails on `ledcAttach`/`ledcSetup` | The installed Arduino-ESP32 core version determines which LEDC API exists; the drivers here use the older `ledcSetup`/`ledcAttachPin`/`ledcWrite(channel, ...)` API. |
