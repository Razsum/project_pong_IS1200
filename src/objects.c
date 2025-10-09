#include "objects.h"
#include "graphics.h"

const int pad_w = 6, pad_h = 50;
int p1x = 10, p1y = HEIGHT / 2 - pad_h / 2;
int p2x = WIDTH - 16, p2y = HEIGHT / 2 - pad_h / 2;
int prev_p1x = 10, prev_p1y = HEIGHT / 2 - pad_h / 2;
int prev_p2x = WIDTH - 16, prev_p2y = HEIGHT / 2 - pad_h / 2;
int d1y = 0, d2y = 0;

int ball_vel = 2;
const int ball_sz = 5;
float bx = WIDTH / 2 - ball_sz / 2.0f, by = HEIGHT / 2 - ball_sz / 2.0f;
float prev_bx = WIDTH / 2 - ball_sz / 2.0f, prev_by = HEIGHT / 2 - ball_sz / 2.0f;
float ball_dx = 2;
float ball_dy = 0;

void initialize_ball(int vel)
{
  // Use frame counter as seed for randomness
  simple_srand(frame_counter * 7919);

  ball_vel = vel;

  int r_x = simple_rand() % 2;
  int r_y = simple_rand() % 2;

  if (r_x)
  {
    ball_dx = -ball_dx;
  }

  if (r_y)
  {
    ball_dy = -ball_dy;
  }
}

void reset_ball_position()
{
  bx = WIDTH / 2 - ball_sz / 2.0f, by = HEIGHT / 2 - ball_sz / 2.0f;
  prev_bx = WIDTH / 2 - ball_sz / 2.0f, prev_by = HEIGHT / 2 - ball_sz / 2.0f;
  ball_dx = 2;
  ball_dy = 0;
}

void reset_player_position()
{
  p1x = 10, p1y = HEIGHT / 2 - pad_h / 2;
  p2x = WIDTH - 16, p2y = HEIGHT / 2 - pad_h / 2;
  prev_p1x = 10, prev_p1y = HEIGHT / 2 - pad_h / 2;
  prev_p2x = WIDTH - 16, prev_p2y = HEIGHT / 2 - pad_h / 2;
  d1y = 0, d2y = 0;
}

void update_ball_physics(int *p1_score, int *p2_score)
{
  prev_bx = bx;
  prev_by = by;
  bx += ball_dx;
  by += ball_dy;

  // Ball hits left paddle
  if (bx <= p1x + pad_w && (by + ball_sz >= p1y && by <= p1y + pad_h) && ball_dx < 0)
  {
    bx = p1x + pad_w;
    float hit_position = (by - (p1y + pad_h / 2.0f)) / (pad_h / 2.0f);

    if (hit_position < -1.0f)
      hit_position = -1.0f;
    if (hit_position > 1.0f)
      hit_position = 1.0f;

    // Calculate bounce angle based on hit position
    float max_bounce_angle = M_PI / 4.0f;
    float bounce_angle = hit_position * max_bounce_angle;

    // Set new velocity with angle
    ball_dx = ball_vel * simple_cos(bounce_angle);
    ball_dy = ball_vel * simple_sin(bounce_angle);

    // Clamp vertical speed
    if (ball_dy > 3.0f)
      ball_dy = 3.0f;
    if (ball_dy < -3.0f)
      ball_dy = -3.0f;
  }

  // Ball hits right paddle
  if (bx + ball_sz >= p2x && (by + ball_sz >= p2y && by <= p2y + pad_h) && ball_dx > 0)
  {
    bx = p2x - ball_sz;
    float hit_position = (by - (p2y + pad_h / 2.0f)) / (pad_h / 2.0f);

    if (hit_position < -1.0f)
      hit_position = -1.0f;
    if (hit_position > 1.0f)
      hit_position = 1.0f;

    // Calculate bounce angle based on hit position
    float max_bounce_angle = M_PI / 4.0f;
    float bounce_angle = hit_position * max_bounce_angle;

    // Set new velocity with angle
    ball_dx = -ball_vel * simple_cos(bounce_angle);
    ball_dy = ball_vel * simple_sin(bounce_angle);

    // Clamp vertical speed
    if (ball_dy > 3.0f)
      ball_dy = 3.0f;
    if (ball_dy < -3.0f)
      ball_dy = -3.0f;
  }

  // Ball hits top or bottom boundary
  if (by <= 0 || by + ball_sz >= HEIGHT)
  {
    ball_dy = -ball_dy;
    if (by <= 0)
      by = 0;
    if (by + ball_sz >= HEIGHT)
      by = HEIGHT - ball_sz;
  }

  // Updates the score if ball hits left or right boundary
  if (bx <= 0)
  {
    (*p2_score)++;
    bx = WIDTH / 2 - ball_sz / 2;
    by = HEIGHT / 2 - ball_sz / 2;
    ball_dx = ball_vel;
    ball_dy = 0.0f;
    frame_counter++;
    initialize_ball(ball_vel);
  }

  if (bx >= WIDTH)
  {
    (*p1_score)++;
    bx = WIDTH / 2 - ball_sz / 2;
    by = HEIGHT / 2 - ball_sz / 2;
    ball_dx = -ball_vel;
    ball_dy = 0.0f;
    frame_counter++;
    initialize_ball(ball_vel);
  }
}

void update_player_position()
{
  // Updates the paddle's width and height 
  // Stays consistent with current position
  prev_p1x = p1x;
  prev_p1y = p1y;
  prev_p2x = p2x;
  prev_p2y = p2y;

  // Updates the moving direction of the paddles
  p1y += d1y;
  p2y += d2y;

  // Stops the paddles from going out of bounds
  if (p1y < 0)
    p1y = 0;
  if (p1y + pad_h > HEIGHT)
    p1y = HEIGHT - pad_h;

  if (p2y < 0)
    p2y = 0;
  if (p2y + pad_h > HEIGHT)
    p2y = HEIGHT - pad_h;
}