#include "sprites.h"

static const uint32_t palette[256] = {
    [0] = 0x302f2f,   // dark gray
    [1] = 0xffdb27,   // yellow
    [2] = 0xffffff,   // white
    [3] = 0x1bacfb,   // blue
};

/* Paddle Logic */
#define PADDLE_WIDTH  6
#define PADDLE_HEIGHT 50

static uint8_t sprite_paddle_bitmap[PADDLE_WIDTH * PADDLE_HEIGHT];

void initialize_paddle_bitmap(void) {
    for (int y = 0; y < PADDLE_HEIGHT; y++) {
        for (int x = 0; x < PADDLE_WIDTH; x++) {
            sprite_paddle_bitmap[y * PADDLE_WIDTH + x] = 2;
        }
    }
}

static const uint8_t sprite_ball_bitmap[8*8]= {
{
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 1, 1, 1, 1, 0, 0, 
0, 1, 2, 1, 1, 2, 1, 0, 
0, 1, 1, 1, 1, 1, 1, 0, 
0, 1, 2, 1, 1, 2, 1, 0, 
0, 1, 1, 2, 2, 1, 1, 0, 
0, 0, 1, 1, 1, 1, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0,
}
};

static const uint8_t sprite_goalKeeper_bitmap[8*8]= {
{
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 3, 2, 2, 3, 0, 0, 
0, 3, 3, 2, 2, 3, 3, 0, 
0, 3, 3, 2, 2, 3, 3, 0, 
0, 3, 3, 2, 2, 3, 3, 0, 
0, 3, 3, 2, 2, 3, 3, 0, 
0, 0, 3, 2, 2, 3, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0,
}
};

static const uint8_t sprite_speedUp_bitmap[8*8]= {
{
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 3, 2, 2, 3, 0, 0, 
0, 3, 2, 3, 3, 3, 3, 0, 
0, 2, 3, 3, 2, 3, 3, 0, 
0, 2, 3, 3, 2, 3, 3, 0, 
0, 3, 2, 3, 3, 3, 3, 0, 
0, 0, 3, 2, 2, 3, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0,
}
};

static const uint8_t sprite_doubleBall_bitmap[8*8]= {
{
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 2, 2, 2, 2, 0, 0, 
0, 2, 1, 2, 2, 1, 2, 0, 
0, 2, 2, 2, 2, 2, 2, 0, 
0, 2, 1, 2, 2, 1, 2, 0, 
0, 2, 2, 1, 1, 2, 2, 0, 
0, 0, 2, 2, 2, 2, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0,
}
};

static const sprite_t sprite_ball       = { .w = 8, .h = 8, .bitmap = sprite_ball_bitmap };
static const sprite_t sprite_paddle     = { .w = 6, .h = 50, .bitmap = sprite_paddle_bitmap };
static const sprite_t sprite_goalkeeper = { .w = 8, .h = 8, .bitmap = sprite_goalKeeper_bitmap };
static const sprite_t sprite_speedUp    = { .w = 8, .h = 8, .bitmap = sprite_speedUp_bitmap };
static const sprite_t sprite_doubleBall = { .w = 8, .h = 8, .bitmap = sprite_doubleBall_bitmap };


const sprite_t* get_sprite(int spriteID) {
    switch(spriteID) {
        case 0: return &sprite_goalkeeper;
        case 1: return &sprite_speedUp;
        case 2: return &sprite_doubleBall;
        case 3: return &sprite_paddle;
        case 4: return &sprite_ball;
    }
}