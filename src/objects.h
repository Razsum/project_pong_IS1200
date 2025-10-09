#ifndef OBJECTS_H
#define OBJECTS_H

#include "simple-math.h"

#define WIDTH 320
#define HEIGHT 240

// Paddle object
extern const int pad_w, pad_h;
extern int p1x, p1y;
extern int p2x, p2y;
extern int prev_p1x, prev_p1y;
extern int prev_p2x, prev_p2y;
extern int d1y, d2y;

// Ball object
extern const int ball_sz;
extern int ball_vel;
extern float bx, by;
extern float prev_bx, prev_by;
extern float ball_dx, ball_dy;

// Ball functions
extern void initialize_ball(int vel);
extern void reset_ball_position();
extern void update_ball_physics(int *p1_score, int *p2_score);

// Paddle functions
extern void reset_player_position();
extern void update_player_position();

#endif