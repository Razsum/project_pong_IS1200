#include "graphics.h"

const uint8_t digits[10][7] = {
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

static const uint8_t letters[26][7] = {
    {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}, // A
    {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E}, // B
    {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E}, // C
    {0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E}, // D
    {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F}, // E
    {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10}, // F
    {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F}, // G
    {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}, // H
    {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E}, // I
    {0x07, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0C}, // J
    {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11}, // K
    {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F}, // L
    {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11}, // M
    {0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11}, // N
    {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}, // O
    {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10}, // P
    {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D}, // Q
    {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11}, // R
    {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E}, // S
    {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}, // T
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}, // U
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04}, // V
    {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11}, // W
    {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11}, // X
    {0x11, 0x11, 0x11, 0x0A, 0x04, 0x04, 0x04}, // Y
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F}  // Z
};

// 8-bit framebuffer pointer (1 byte per pixel)
volatile uint8_t *const fb = (volatile uint8_t *)FB_BASE;

// Pixels & rects
inline void pset8(int x, int y, uint8_t c)
{
  if ((unsigned)x < WIDTH && (unsigned)y < HEIGHT)
    fb[y * WIDTH + x] = c;
}

void rect_fill8(int x,int y,int w,int h,uint8_t c){
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
void draw_digit(int x, int y, int digit, uint8_t color) {
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
void draw_score(int x, int y, int score, uint8_t color) {
    int tens = (score / 10) % 10;
    int ones = score % 10;
    
    draw_digit(x, y, tens, color);
    draw_digit(x + 8, y, ones, color);
}

// Draw a single character (letter, digit, or space)
void draw_char(int x, int y, char c, uint8_t color) {
    if (c >= 'A' && c <= 'Z') {
        // Uppercase letter
        int idx = c - 'A';
        for (int row = 0; row < 7; row++) {
            uint8_t line = letters[idx][row];
            for (int col = 0; col < 5; col++) {
                if (line & (1 << (4 - col))) {
                    pset8(x + col, y + row, color);
                }
            }
        }
    } else if (c >= 'a' && c <= 'z') {
        // Convert lowercase to uppercase
        draw_char(x, y, c - 32, color);
    } else if (c >= '0' && c <= '9') {
        // Digit
        draw_digit(x, y, c - '0', color);
    }
}

// Draw a string of text
void draw_text(int x, int y, const char *text, uint8_t color) {
    int cursor_x = x;
    while (*text) {
        draw_char(cursor_x, y, *text, color);
        cursor_x += 6;  // 5 pixels wide + 1 pixel spacing
        text++;
    }
}

void draw_all(int p1_score, int p2_score) {
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

void clear_screen8(uint8_t c)
{
  for (int i = 0; i < WIDTH * HEIGHT; ++i)
    fb[i] = c;
}