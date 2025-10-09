#include "dtekv-lib.h"
#include "dtekv-mpu6050-lib/dtekv-mpu6050-lib.h"
#include "dtekv-i2c-lib/dtekv-i2c-lib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "objects.h"
#include "simple-math.h"

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
    update_player_position();
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
