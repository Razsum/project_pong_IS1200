// pong_ui_static_8bpp.c — DTEK-V Pong UI (static, no controls), 8-bit framebuffer
#include <stdint.h>
#include <stdbool.h>
#include "sprites\sprites.h"

#define FB_BASE 0x08000000u
#define WIDTH 320
#define HEIGHT 240

enum {
COL_BG   = 0x00  // background (black)
};

// Ball state
int ballX, ballY;
int ballDX = 1, ballDY = 1;

/* 8-bit framebuffer pointer (1 byte per pixel) */
static volatile uint8_t *const fb = (volatile uint8_t *)FB_BASE;
void render_sprite(int x, int y, int spriteID);

static void clear_screen(uint8_t c)
{
    for (int i = 0; i < WIDTH * HEIGHT; ++i)
        fb[i] = c;
}

void ball_spawn()
{
    ballX = WIDTH / 2 - 16;
    ballY = HEIGHT / 2 - 16;
    render_sprite(ballX, ballY, 0);
}

void ball_movement()
{
    // erase old ball 
    for (int j = 0; j < 32; j++)
        for (int i = 0; i < 32; i++)
        {
            int px = ballX + i;
            int py = ballY + j;
            if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT)
                fb[py * WIDTH + px] = COL_BG;
        }

    // update position
    ballX += ballDX;
    ballY += ballDY;

    // bounece off edges
    if (ballX < 0 || ballX + 32 > WIDTH)
        ballDX = -ballDX;
    if (ballY < 0 || ballY + 32 > HEIGHT)
        ballDY = -ballDY;

    // redraw ball
    render_sprite(ballX, ballY, 0);
}

static inline void delay_cycles(volatile uint32_t n) { while (n--) __asm__ volatile("");}; 

int main(void)
{
    clear_screen(COL_BG);
    ball_spawn();

    while (1)
    {
        ball_movement();
        delay_cycles(80000);
    }
}
