#include "dtekv-lib.h"
#include "dtekv-mpu6050-lib/dtekv-mpu6050-lib.h"
#include "dtekv-i2c-lib/dtekv-i2c-lib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <objects.h>

#define SENSITIVITY 180
#define M_PI 3.14159265358979323846

#define FB_BASE       0x08000000u
#define WIDTH         320
#define HEIGHT        240
#define GPIO_IN_ADDR  0x040000E0u
#define GPIO_IN       (*(volatile uint32_t*)GPIO_IN_ADDR)

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

// Simple cosine approximation (good enough for small angles)
static float simple_cos(float x) {
    // Taylor series: cos(x) ≈ 1 - x²/2 + x⁴/24
    float x2 = x * x;
    return 1.0f - x2 * 0.5f + x2 * x2 * 0.041666f;
}

static float simple_sin(float x) {
    // Taylor series: sin(x) ≈ x - x³/6 + x⁵/120
    float x2 = x * x;
    return x * (1.0f - x2 * 0.166666f + x2 * x2 * 0.008333f);
}

// Simple pseudo-random number generator
static uint32_t rng_state = 12345;

static void simple_srand(uint32_t seed) {
    rng_state = seed;
}

static uint32_t simple_rand(void) {
    // Linear congruential generator
    rng_state = rng_state * 1103515245 + 12345;
    return (rng_state / 65536) % 32768;
}

/* 8-bit framebuffer pointer (1 byte per pixel) */
static volatile uint8_t *const fb = (volatile uint8_t *)FB_BASE;

/* Palette indices (tweak if needed) */
enum
{
  COL_BG = 0x00,  // background (black)
  COL_NET = 0x80, // mid gray (center net)
  COL_FG = 0xFF,  // paddles
  COL_BALL = 0xC0 // ball
};

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

static void rect_fill8(int x,int y,int w,int h,uint8_t c){
  if (w<=0||h<=0) return;
  if (x<0) { w+=x; x=0; } if (y<0) { h+=y; y=0; }
  if (x+w>WIDTH)  w=WIDTH-x;
  if (y+h>HEIGHT) h=HEIGHT-y;
  
  for (int j = 0; j < h; ++j) {
    volatile uint8_t* row = fb + (y+j)*WIDTH + x;
    for(int i=0;i<w;++i) row[i]=c;
  }
}

// Draw a single digit at position (x, y)
static void draw_digit(int x, int y, int digit, uint8_t color) {
    if (digit < 0 || digit > 9) return;
    
    for (int row = 0; row < 7; row++) {
        uint8_t line = digits[digit][row];
        for (int col = 0; col < 5; col++) {
            if (line & (1 << (4 - col))) {
                pset8(x + col, y + row, color);
            }
        }
    }
}

// Draw a two-digit number (00-99)
static void draw_score(int x, int y, int score, uint8_t color) {
    int tens = (score / 10) % 10;
    int ones = score % 10;
    
    draw_digit(x, y, tens, color);
    draw_digit(x + 8, y, ones, color);  // 8 pixels apart (5 + gap)
}

static void draw_all(int p1_score, int p2_score) {
  // erase old paddle areas
  rect_fill8(prev_p1x, prev_p1y, pad_w, pad_h, COL_BG);
  rect_fill8(prev_p2x, prev_p2y, pad_w, pad_h, COL_BG);

  // erase old ball area
  rect_fill8(prev_bx, prev_by, ball_sz, ball_sz, COL_BG);

  // erase score areas
  rect_fill8(WIDTH/2 - 40, 10, 16, 7, COL_BG);
  rect_fill8(WIDTH/2 + 20, 10, 16, 7, COL_BG);

  // redraw net slice
  for(int y=0;y<HEIGHT;y+=8) rect_fill8(WIDTH/2-1, y, 2, 4, COL_NET);

  // redraw paddles at new positions
  rect_fill8(p1x, p1y, pad_w, pad_h, COL_FG);
  rect_fill8(p2x, p2y, pad_w, pad_h, COL_FG);

  // redraw ball at new position
  rect_fill8(bx, by, ball_sz, ball_sz, COL_BALL);

  // redraw score
  draw_score(WIDTH/2 - 40, 10, p1_score, COL_FG);
  draw_score(WIDTH/2 + 20, 10, p2_score, COL_FG);
}

static uint32_t frame_counter = 0;

static void initialize_ball() {
  // Use frame counter as seed for randomness
  simple_srand(frame_counter * 7919);
  
  int r_x = simple_rand() % 2;
  int r_y = simple_rand() % 2;

  if (r_x) {
    ball_dx = -ball_dx;
  }

  if (r_y) {
    ball_dy = -ball_dy;
  }
}

static void update_ball_physics(int *p1_score, int *p2_score) {
    prev_bx = bx;
    prev_by = by;
    bx += ball_dx;
    by += ball_dy;

    // Ball hits left paddle
    if (bx <= p1x + pad_w && (by + ball_sz >= p1y && by <= p1y + pad_h) && ball_dx < 0) {
      bx = p1x + pad_w;
      float hit_position = (by - (p1y + pad_h/2.0f)) / (pad_h / 2.0f);

      if (hit_position < -1.0f) hit_position = -1.0f;
      if (hit_position > 1.0f) hit_position = 1.0f;


      // Calculate bounce angle based on hit position
      float max_bounce_angle = M_PI / 4.0f;
      float bounce_angle = hit_position * max_bounce_angle;
      
      // Set new velocity with angle
      ball_dx = ball_vel * simple_cos(bounce_angle);
      ball_dy = ball_vel * simple_sin(bounce_angle);
      
      // Clamp vertical speed
      if (ball_dy > 3.0f) ball_dy = 3.0f;
      if (ball_dy < -3.0f) ball_dy = -3.0f;

    }

    // Ball hits right paddle
    if (bx + ball_sz >= p2x && (by + ball_sz >= p2y && by <= p2y + pad_h) && ball_dx > 0) {
      bx = p2x - ball_sz;
      float hit_position = (by - (p2y + pad_h/2.0f)) / (pad_h / 2.0f);

      if (hit_position < -1.0f) hit_position = -1.0f;
      if (hit_position > 1.0f) hit_position = 1.0f;

      // Calculate bounce angle based on hit position
      float max_bounce_angle = M_PI / 4.0f;
      float bounce_angle = hit_position * max_bounce_angle;
      
      // Set new velocity with angle
      ball_dx = -ball_vel * simple_cos(bounce_angle);
      ball_dy = ball_vel * simple_sin(bounce_angle);
      
      // Clamp vertical speed
      if (ball_dy > 3.0f) ball_dy = 3.0f;
      if (ball_dy < -3.0f) ball_dy = -3.0f;
    }

    // Ball hits wall
    if (by <= 0 || by + ball_sz >= HEIGHT) {
      ball_dy = -ball_dy;
      if (by <= 0) by = 0;
      if (by + ball_sz >= HEIGHT) by = HEIGHT - ball_sz;
    }

    if (bx <= 0) {
      (*p2_score)++;
      bx = WIDTH/2 - ball_sz/2;
      by = HEIGHT/2 - ball_sz/2;
      ball_dx = ball_vel;
      ball_dy = 0.0f;
      frame_counter++;
      initialize_ball();
    }

    if (bx >= WIDTH) {
      (*p1_score)++;
      bx = WIDTH/2 - ball_sz/2;
      by = HEIGHT/2 - ball_sz/2;
      ball_dx = -ball_vel;
      ball_dy = 0.0f;
      frame_counter++;
      initialize_ball();
    }
}

static void update_player_position(int d1y, int d2y) {
  prev_p1x = p1x;
  prev_p1y = p1y;
  prev_p2x = p2x;
  prev_p2y = p2y;

  p1y += d1y;
  p2y += d2y;
  if (p1y < 0) p1y = 0;
  if (p1y + pad_h > HEIGHT) p1y = HEIGHT - pad_h;

  if (p2y < 0) p2y = 0;
  if (p2y + pad_h > HEIGHT) p2y = HEIGHT - pad_h;
}

static void clear_screen8(uint8_t c)
{
  for (int i = 0; i < WIDTH * HEIGHT; ++i)
    fb[i] = c;
}

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
    
    // Player 1 controls
    if (y1 < -10) {          // Tilted one way
        d1y = -1;            // Move up
    } else if (y1 > 10) {    // Tilted other way  
        d1y = 1;             // Move down
    }
    
    // Player 2 controls
    if (y2 < -10) {          // Tilted one way
        d2y = -1;            // Move up
    } else if (y2 > 10) {    // Tilted other way  
        d2y = 1;             // Move down
    }
    
    // ---- UPDATE ----
    update_ball_physics(&p1_score, &p2_score);
    update_player_position(d1y, d2y);
    draw_all(p1_score, p2_score);

    wait(5);
  }

  if (p1_score == 10) {
      print("Player 1 wins!");
  }
  if (p2_score == 10) {
    print("Player 2 wins!");
  }
}
