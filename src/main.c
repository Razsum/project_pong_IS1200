#include "objects.h"
#include "power_up.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "dtekv-lib.h"
#include "dtekv-mpu6050-lib/dtekv-mpu6050-lib.h"
#include "dtekv-i2c-lib/dtekv-i2c-lib.h"

#define SENSITIVITY 180
#define M_PI 3.14159265358979323846

#define FB_BASE 0x08000000u

#define GPIO_IN_ADDR 0x040000E0u
#define GPIO_IN (*(volatile uint32_t *)GPIO_IN_ADDR)

static const uint8_t digits[10][7] = {
    {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}, // 0
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}, // 1
    {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F}, // 2
    {0x0E, 0x11, 0x01, 0x0E, 0x01, 0x11, 0x0E}, // 3
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}, // 4
    {0x1F, 0x10, 0x10, 0x1E, 0x01, 0x11, 0x0E}, // 5
    {0x0E, 0x11, 0x10, 0x1E, 0x11, 0x11, 0x0E}, // 6
    {0x1F, 0x01, 0x02, 0x04, 0x04, 0x04, 0x04}, // 7
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}, // 8
    {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x11, 0x0E}  // 9
};

/**
 * Angel approximation to calculate ball trajectory
 */
// Cosine approximation
static float simple_cos(float x)
{
  // Taylor series: cos(x) ≈ 1 - x²/2 + x⁴/24
  float x2 = x * x;
  return 1.0f - x2 * 0.5f + x2 * x2 * 0.041666f;
}
// Sinus approximation
static float simple_sin(float x)
{
  // Taylor series: sin(x) ≈ x - x³/6 + x⁵/120
  float x2 = x * x;
  return x * (1.0f - x2 * 0.166666f + x2 * x2 * 0.008333f);
}

// Pseudo-random number generator
static uint32_t rng_state = 12345;

static void simple_srand(uint32_t seed)
{
  rng_state = seed;
}

static uint32_t simple_rand(void)
{
  // Linear congruential generator
  rng_state = rng_state * 1103515245 + 12345;
  return (rng_state / 65536) % 32768;
}

/* 8-bit framebuffer pointer (1 byte per pixel) */
static volatile uint8_t *const fb = (volatile uint8_t *)FB_BASE;

/* Palette indices */
enum
{
  COL_BG = 0x00,  // background (black)
  COL_NET = 0x80, // mid gray (center net)
  COL_FG = 0xFF,  // paddles
  COL_BALL = 0xC0 // ball
};

/* Wait function */
static void wait(unsigned short ms)
{
  for (unsigned int i = 0; i < 10000 * ms; i++)
    asm volatile("nop");
}

/* Pixels & rects */
static inline void pset8(int x, int y, uint8_t c)
{
  if ((unsigned)x < WIDTH && (unsigned)y < HEIGHT)
    fb[y * WIDTH + x] = c;
}

static void rect_fill8(int x, int y, int w, int h, uint8_t c)
{
  if (w <= 0 || h <= 0)
    return;
  if (x < 0)
  {
    w += x;
    x = 0;
  }
  if (y < 0)
  {
    h += y;
    y = 0;
  }
  if (x + w > WIDTH)
    w = WIDTH - x;
  if (y + h > HEIGHT)
    h = HEIGHT - y;

  for (int j = 0; j < h; ++j)
  {
    volatile uint8_t *row = fb + (y + j) * WIDTH + x;
    for (int i = 0; i < w; ++i)
      row[i] = c;
  }
}

/* Draw a single digit at position (x, y) */
static void draw_digit(int x, int y, int digit, uint8_t color)
{
  if (digit < 0 || digit > 9)
    return;

  for (int row = 0; row < 7; row++)
  {
    uint8_t line = digits[digit][row];
    for (int col = 0; col < 5; col++)
    {
      if (line & (1 << (4 - col)))
      {
        pset8(x + col, y + row, color);
      }
    }
  }
}

/* Draw a two-digit number (00-99) */
static void draw_score(int x, int y, int score, uint8_t color)
{
  int tens = (score / 10) % 10;
  int ones = score % 10;

  draw_digit(x, y, tens, color);
  draw_digit(x + 8, y, ones, color);
}

/* Drawing Function - Erases and Redraws*/
static void draw_all(int p1_score, int p2_score, PowerUp power_up)
{
  // Erase paddles
  rect_fill8(prev_p1x, prev_p1y, pad_w, pad_h, COL_BG);
  rect_fill8(prev_p2x, prev_p2y, pad_w, pad_h, COL_BG);

  // Erase ball
  rect_fill8(prev_bx, prev_by, ball_sz, ball_sz, COL_BG);

  // Erase score area
  rect_fill8(WIDTH / 2 - 40, 10, 16, 7, COL_BG);
  rect_fill8(WIDTH / 2 + 20, 10, 16, 7, COL_BG);

  rect_fill8(&power_up.x, &power_up.y, power_sz, power_sz, COL_BG);

  // Redraws net
  for (int y = 0; y < HEIGHT; y += 8)
    rect_fill8(WIDTH / 2 - 1, y, 2, 4, COL_NET);

  // Redraws paddles at new position
  rect_fill8(p1x, p1y, pad_w, pad_h, COL_FG);
  rect_fill8(p2x, p2y, pad_w, pad_h, COL_FG);

  // Redraws ball at new position
  rect_fill8(bx, by, ball_sz, ball_sz, COL_BALL);

  // Redraws score
  draw_score(WIDTH / 2 - 40, 10, p1_score, COL_FG);
  draw_score(WIDTH / 2 + 20, 10, p2_score, COL_FG);

  rect_fill8(&power_up.x, &power_up.y, power_sz, power_sz, COL_BALL);

}

void draw_powerup(int *px, int *py, int p_type) {
    rect_fill8(*px, *py, power_sz, power_sz, COL_BALL);
}

/* Frame Counter */
uint32_t frame_counter = 0;

/* Initializes Ball and Draws it at the correct positon */
static void initialize_ball()
{
  // Use frame counter as seed for randomness
  simple_srand(frame_counter * 7919);

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

/* Gives ball physics and interactions */
static void update_ball_physics(int *p1_score, int *p2_score, PowerUp power_up)
{
  prev_bx = bx;
  prev_by = by;
  bx += ball_dx;
  by += ball_dy;

  // Ball hits left paddle
  if (bx <= p1x + pad_w && (by + ball_sz >= p1y && by <= p1y + pad_h) && ball_dx < 0)
  {
    power_up_position(&power_up.x, &power_up.y, 2, 1);
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
    //player_ball(2);
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

  // Ball hits wall
  if (by <= 0 || by + ball_sz >= HEIGHT)
  {
    ball_dy = -ball_dy;
    if (by <= 0)
      by = 0;
    if (by + ball_sz >= HEIGHT)
      by = HEIGHT - ball_sz;
  }
  // Ball hits left-wall (p2 score)
  if (bx <= 0)
  {
    (*p2_score)++;
    bx = WIDTH / 2 - ball_sz / 2;
    by = HEIGHT / 2 - ball_sz / 2;
    ball_dx = ball_vel;
    ball_dy = 0.0f;
    frame_counter++;
    initialize_ball();
  }
  // Ball hits right-wall (p1 score)
  if (bx >= WIDTH)
  {
    (*p1_score)++;
    bx = WIDTH / 2 - ball_sz / 2;
    by = HEIGHT / 2 - ball_sz / 2;
    ball_dx = -ball_vel;
    ball_dy = 0.0f;
    frame_counter++;
    initialize_ball();
  }
}

/* Keeps track of hte current position of each paddle */
static void update_player_position(int d1y, int d2y)
{
  prev_p1x = p1x;
  prev_p1y = p1y;
  prev_p2x = p2x;
  prev_p2y = p2y;

  p1y += d1y;
  p2y += d2y;
  if (p1y < 0)
    p1y = 0;
  if (p1y + pad_h > HEIGHT)
    p1y = HEIGHT - pad_h;

  if (p2y < 0)
    p2y = 0;
  if (p2y + pad_h > HEIGHT)
    p2y = HEIGHT - pad_h;
}

/* Clear screen - Blank */
static void clear_screen8(uint8_t c)
{
  for (int i = 0; i < WIDTH * HEIGHT; ++i)
    fb[i] = c;
}

/* Prints out current score */
static void prints(short s)
{
  if (s < 0)
  {
    printc('-');
    print_dec(-s);
  }
  else
    print_dec(s);
}

void handle_interrupt(void) {}

int main()
{
  int p1_score = 0, p2_score = 0;

  initializeSensor(0);
  initializeSensor(1);

  clear_screen8(COL_BG);

  // Draw net once
  for (int y = 0; y < HEIGHT; y += 8)
    rect_fill8(WIDTH / 2 - 1, y, 2, 4, COL_NET);

  // Draws paddles and ball
  rect_fill8(p1x, p1y, pad_w, pad_h, COL_FG);
  rect_fill8(p2x, p2y, pad_w, pad_h, COL_FG);
  rect_fill8(bx, by, ball_sz, ball_sz, COL_BALL);

  short x1 = 0;
  short y1 = 0;
  short x2 = 0;
  short y2 = 0;

  initialize_ball();

  while (p1_score < 10 && p2_score < 10)
  {
    getAccelerometer(0, &x1, &y1);
    getAccelerometer(1, &x2, &y2);

    y1 = y1 / SENSITIVITY;
    y2 = y2 / SENSITIVITY;
    int d1y = 0;
    int d2y = 0;

    // Player 1 controls
    if (y1 < -10)
    {           // Tilted one way
      d1y = -1; // Move up
    }
    else if (y1 > 10)
    {          // Tilted other way
      d1y = 1; // Move down
    }

    // Player 2 controls
    if (y2 < -10)
    {           // Tilted one way
      d2y = -1; // Move up
    }
    else if (y2 > 10)
    {          // Tilted other way
      d2y = 1; // Move down
    }

    print(" Y1=");
    prints(y1);
    print(" Y2=");
    prints(y2);
    print("\n");

    // Updates position
    PowerUp power_up = rand_power_up();
    prints(power_up.x);
    prints(power_up.y);

    draw_all(p1_score, p2_score, power_up);
    update_ball_physics(&p1_score, &p2_score, power_up);
    update_player_position(d1y, d2y);

    wait(5);
  }
  // Victory condition
  if (p1_score == 10)
  {
    print("Player 1 wins!");
  }
  if (p2_score == 10)
  {
    print("Player 2 wins!");
  }
}