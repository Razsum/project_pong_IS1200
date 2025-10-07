#ifndef SPRITES_H
#define SPRITES_H

#include <stdint.h>

typedef struct {
    uint8_t w, h;
    const uint8_t *bitmap; 
} sprite_t;

// Returns a pointer to a sprite by ID
const sprite_t* get_sprite(int spriteID);
void render_sprite(int x, int y, int spriteID);

#endif