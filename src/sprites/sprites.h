#ifndef SPRITES_H
#define SPRITES_H

#include <stdint.h>

typedef struct {
    int w, h;
    const uint8_t *bitmap; 
} sprite_t;

// Returns a pointer to a sprite by ID
const sprite_t* get_sprite(int spriteID);

#endif