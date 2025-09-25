#include "sprites.h"

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

static const sprite_t sprite_ball = {
    .w = 8,
    .h = 8,
    .bitmap = sprite_ball_bitmap
};


const sprite_t* get_sprite(int spriteID) {
    switch(spriteID) {
        case 0: return &sprite_ball;
        default: return 0;
    }
}

// 0x302f2f = 0
// 0xff11dbe6 = 1
// 0x302f2f = 2