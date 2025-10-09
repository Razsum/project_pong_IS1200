#ifndef OBJECTS_H
#define OBJECTS_H

#include "simple-math.h"

/**
 * @brief Defines all paddle object attributes
 * Defines position, size, and movement variables for both player paddles
 * @author Razmus Nilsson
 */
extern const int pad_w, pad_h;
extern int p1x, p1y;
extern int p2x, p2y;
extern int prev_p1x, prev_p1y;
extern int prev_p2x, prev_p2y;
extern int d1y, d2y;

/**
 * @brief Defines all ball object attributes
 * Defines position, size, and movement variables for the ball
 * @author Razmus Nilsson
 */
extern const int ball_sz;
extern int ball_vel;
extern float bx, by;
extern float prev_bx, prev_by;
extern float ball_dx, ball_dy;

/**
 * @brief Initializes the ball, uses simple_rand to determine a pseudo-random starting dx/dy direction
 *
 * @param vel Takes in the current velocity of the ball
 * @authors Kai Nguyen & Razmus Nilsson
 */
extern void initialize_ball(int vel);

/**
 * @brief Resets the ball to the middle of the screen, with horizontal speed of dx = 2
 *
 * @author Razmus Nilsson
 *
 */
extern void reset_ball_position();

/**
 * @brief Handles all logic for ball movement, collisions, and scoring.
 *
 * Detects collisions with paddles and screen edges, updates the ball’s
 * position and velocity, and increments player scores when the ball
 * touches the adjacent player's goal
 *
 * @param p1_score Pointer to player 1's score; incremented when player 1 scores.
 * @param p2_score Pointer to player 2's score; incremented when player 2 scores.
 * @author Kai Nguyen
 */
extern void update_ball_physics(int *p1_score, int *p2_score);

/**
 * @brief Resets the paddles in their respective positons opposite of each other
 *
 * @author Razmus Nilsson
 */
extern void reset_player_position();

/**
 * @brief Updates the player (paddles) position during the gameplay loop
 *
 * @author Razmus Nilsson
 */
extern void update_player_position();

#endif