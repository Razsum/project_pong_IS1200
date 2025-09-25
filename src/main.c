// pong_ui_static_8bpp.c — DTEK-V Pong UI (static, no controls), 8-bit framebuffer
#include <stdint.h>
#include <stdbool.h>

#define FB_BASE  0x08000000u
#define WIDTH    320
#define HEIGHT   240

void handle_interrupt(unsigned cause) { (void)cause; }

/* 8-bit framebuffer pointer (1 byte per pixel) */
static volatile uint8_t *const fb = (volatile uint8_t*)FB_BASE;

/* ---- Choose palette indices (tweak if your colors look different) ---- */
enum {
  COL_BG   = 0x00,  // background (black)
  COL_NET  = 0x80,  // mid gray (center net)
  COL_FG   = 0xFF,  // foreground (paddles, text)
  COL_BALL = 0xC0   // light gray ball
};

/* ---- Pixel / shape helpers (8bpp) ---- */
static inline void pset8(int x, int y, uint8_t c) {
  if ((unsigned)x < WIDTH && (unsigned)y < HEIGHT) fb[y * WIDTH + x] = c;
}

static void rect_fill8(int x, int y, int w, int h, uint8_t c) {
  if (w <= 0 || h <= 0) return;
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }
  if (x + w > WIDTH)  w = WIDTH  - x;
  if (y + h > HEIGHT) h = HEIGHT - y;
  for (int j = 0; j < h; ++j) {
    volatile uint8_t *row = fb + (y + j) * WIDTH + x;
    for (int i = 0; i < w; ++i) row[i] = c;
  }
}

static void clear_screen8(uint8_t c) {
  for (int i = 0; i < WIDTH * HEIGHT; ++i) fb[i] = c;
}

static inline void delay_cycles(volatile uint32_t n) { while (n--) __asm__ volatile(""); }

int main(void) {
  // 1) Clear background
  clear_screen8(COL_BG);

  // 3) Paddles (static positions)
  // Left paddle
  const int pad_w = 6, pad_h = 50;
  int p1x = 10,           p1y = HEIGHT/2 - pad_h/2;
  int p2x = WIDTH - 16,   p2y = HEIGHT/2 - pad_h/2;
  rect_fill8(p1x, p1y, pad_w, pad_h, COL_FG);
  rect_fill8(p2x, p2y, pad_w, pad_h, COL_FG);

  // 4) Ball at center (simple 5x5 square for 8bpp)
  const int ball_sz = 5;
  int bx = WIDTH/2 - ball_sz/2;
  int by = HEIGHT/2 - ball_sz/2;
  rect_fill8(bx, by, ball_sz, ball_sz, COL_BALL);
  
  int dy = 1;  
  
  int prev_p1x = p1x;
  int prev_p1y = p1y;

  // Done: static UI only, no loop needed
  while (1) { 
  	
  	rect_fill8(prev_p1x, prev_p1y, pad_w, pad_h, COL_BG);
  	rect_fill8(p2x, p2y, pad_w, pad_h, COL_FG);
  	rect_fill8(bx, by, ball_sz, ball_sz, COL_BALL);
  	
  	p1y += dy;
  	if (p1y < 0) {p1y = 0; dy = dy;}
  	if (p1y + pad_h > HEIGHT) {p1y = 0; dy = dy;}
  	rect_fill8(p1x, p1y, pad_w, pad_h, COL_FG);

  	prev_p1x = p1x;
  	prev_p1y = p1y;
  	delay_cycles(80000);
  }
}
