# Project Pong IS1200

**Motion-Controlled Pong**

A modern remake of **Pong** with a twist:  
- Motion-controlled paddles using **MPU6050** accelerometer sensors.
- Built entirely in **C** on the **DTEK-V board**  
- Multiplayer exclusive game with Power-Ups.

--- 
# Breakdown

# Project Objective
- **Accelerometer-Sensor Controls**: Each player uses an **MPU6050** accelerometer based controller to move their paddle along the Y axes.
- **Multiplayer**: This is a two-player exclusive game.
- **Power-Ups**: The game will feature randomly generated Power-Ups that will offer an advantage to the player that collects them.

First to reach the winning score condition wins!

# Implementation
- **Platform**: This program is developed and *exclusively* available on the **D-TEK-V board**, entirely programmed in **C**.
- **Input Devices**:
  - Two **MPU6050 Accelerometer-Sensors** each providing X- and Y-axis data input.
  - That input data is read via an **I2C interface** using an I2C DTEK-V library developed by GitHub user Olivoz and translated to movement in-game.
- **Gameplay Mechanics**:
  - Paddles can move within a rectangular area, allowing horizontal and vertical movement.
  - Points are scored when the ball passes an opponent’s boundary.
  - Power-Ups appear randomly at the center of the screen. A player collects them if they were the last to hit the ball and it collides with the Power-Up object.
- **Graphics**: Output is displayed via the DTEK-V’s **VGA interface** on an external screen.

# Verification & Testing
To ensure that the game is behaving correctly with *minimal* bugs and glitches.
- Paddle movement accuracy (Accelerometer → Paddle mapping)
- Proper generation and application of Power-Ups.
- Score tracking and win-condition enforcement.
- Edge cases to verify stability and game flow.
A specification sheet will document key test cases for systematic validation.

# Contributions
- Kai: Communication with IR-Sensors, paddle synchronization, and physics for paddle/ball movement.
- Razmus: Graphical interface, scoring logic, and Power-Up mechanics.
The final workload distribution will be detailed in the project report.

# Optional Features (If time permits)
- Enclosure for breadboard → makeshift physical controllers.
- Extended graphics and sounds effects for the immersion.
- More advanced ball physics (speed based on paddle hit velocity).
