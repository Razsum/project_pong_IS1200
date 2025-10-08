#include "objects.h"

const int pad_w = 6, pad_h = 50;
int p1x = 10, p1y = HEIGHT / 2 - pad_h / 2;
int p2x = WIDTH - 16, p2y = HEIGHT / 2 - pad_h / 2;
int prev_p1x = 10, prev_p1y = HEIGHT / 2 - pad_h / 2;
int prev_p2x = WIDTH - 16, prev_p2y = HEIGHT / 2 - pad_h / 2;

const int ball_vel = 2;
const int ball_sz = 5;
float bx = WIDTH / 2 - ball_sz / 2.0f, by = HEIGHT / 2 - ball_sz / 2.0f;
float prev_bx = WIDTH / 2 - ball_sz / 2.0f, prev_by = HEIGHT / 2 - ball_sz / 2.0f;
float ball_dx = ball_vel;
float ball_dy = 0;

const int power_sz = 5;
