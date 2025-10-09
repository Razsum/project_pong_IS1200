#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "objects.h"

#define FB_BASE       0x08000000u
#define WIDTH         320
#define HEIGHT        240

enum
{
  COL_BG = 0x00,  // background (black)
  COL_NET = 0x80, // mid gray (center net)
  COL_FG = 0xFF,  // paddles
  COL_BALL = 0xC0 // ball
};

extern const uint8_t digits[10][7];
extern volatile uint8_t *const fb;

// Draw functions
extern inline void pset8(int x, int y, uint8_t c);
extern void rect_fill8(int x,int y,int w,int h,uint8_t c);
extern void draw_digit(int x, int y, int digit, uint8_t color);
extern void draw_score(int x, int y, int score, uint8_t color);
extern void draw_all(int p1_score, int p2_score);
extern void clear_screen8(uint8_t c);

#endif