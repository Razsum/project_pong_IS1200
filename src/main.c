#include "dtekv-lib.h"
#include "dtekv-mpu6050-lib/dtekv-mpu6050-lib.h"
#include <stdint.h>
#include <stdbool.h>

#define SENSITIVITY 180

#define FB_BASE       0x08000000u
#define WIDTH         320
#define HEIGHT        240
#define GPIO_IN_ADDR  0x040000E0u
#define GPIO_IN       (*(volatile uint32_t*)GPIO_IN_ADDR)

/* 8-bit framebuffer pointer (1 byte per pixel) */
static volatile uint8_t *const fb = (volatile uint8_t*)FB_BASE;

/* Palette indices (tweak if needed) */
enum {
  COL_BG   = 0x00,  // background (black)
  COL_NET  = 0x80,  // mid gray (center net)
  COL_FG   = 0xFF,  // paddles
  COL_BALL = 0xC0   // ball
};

static void wait(unsigned short ms)
{
    for (unsigned int i = 0; i < 10000 * ms; i++)
        asm volatile("nop");
}

/* Pixels & rects */
static inline void pset8(int x,int y,uint8_t c){
  if((unsigned)x<WIDTH && (unsigned)y<HEIGHT) fb[y*WIDTH + x] = c;
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

static void clear_screen8(uint8_t c){
  for(int i=0;i<WIDTH*HEIGHT;++i) fb[i]=c;
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
    initializeSensor();
    
    clear_screen8(COL_BG);

    // draw net once
    for(int y=0;y<HEIGHT;y+=8) rect_fill8(WIDTH/2-1, y, 2, 4, COL_NET);

    // objects
    const int pad_w=6, pad_h=50;
    int p1x=10,         p1y=HEIGHT/2 - pad_h/2;
    int p2x=WIDTH-16,   p2y=HEIGHT/2 - pad_h/2;
    const int ball_sz=5; int bx=WIDTH/2 - ball_sz/2, by=HEIGHT/2 - ball_sz/2;

    rect_fill8(p1x,p1y,pad_w,pad_h,COL_FG);
    rect_fill8(p2x,p2y,pad_w,pad_h,COL_FG);
    rect_fill8(bx,by,ball_sz,ball_sz,COL_BALL);

    short x = 0;
    short y = 0;
      
    int prev_p1x = p1x, prev_p1y = p1y;

    while (1)
    {
        getAccelerometer(&x, &y);
        y = y / SENSITIVITY;
	int dy = 0;
	
	if (y < -10) {          // Tilted one way
	    dy = -1;            // Move up
	} else if (y > 10) {    // Tilted other way  
	    dy = 1;             // Move down
	}

        print("Y: ");
	prints(y);
	print(" dy: ");
	prints(dy);
	print(" p1y: ");
	prints(p1y);
	print("\n");
        
        // ---- UPDATE ----
        p1y += dy;
        if (p1y < 0) p1y = 0;
        if (p1y + pad_h > HEIGHT) p1y = HEIGHT - pad_h;

        if (p1y != prev_p1y) {  // Only redraw if position changed
	    // erase old paddle area
	    rect_fill8(prev_p1x, prev_p1y, pad_w, pad_h, COL_BG);
	    // redraw net slice
	    for(int y=0;y<HEIGHT;y+=8) rect_fill8(WIDTH/2-1, y, 2, 4, COL_NET);
	    // redraw paddle at new position
	    rect_fill8(p1x, p1y, pad_w, pad_h, COL_FG);
	    // redraw static objects
	    rect_fill8(p2x, p2y, pad_w, pad_h, COL_FG);
	    rect_fill8(bx, by, ball_sz, ball_sz, COL_BALL);
	        
	    prev_p1x = p1x;
	    prev_p1y = p1y;
        }

        wait(5);
    }
}