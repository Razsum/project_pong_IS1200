#include <stdint.h>
#include "sprites.h"

#define WIDTH  320
#define HEIGHT 240


extern volatile uint8_t *const fb;

// Renders sprite onto screen by converting sprite-local coords to framebuffer coords
void render_sprite(int x, int y, int spriteID) {
    const sprite_t *spriteToDraw = get_sprite(spriteID);

    for (int i = 0; i < spriteToDraw->w * spriteToDraw->h; i++) {
        int localX = i % spriteToDraw->w;
        int localY = i / spriteToDraw->w;

        int globalX = x + localX;
        int globalY = y + localY;

        if (globalX < 0 || globalX >= WIDTH) continue;
        if (globalY < 0 || globalY >= HEIGHT) continue;

        // Writes to framebuffer
        fb[globalY * WIDTH + globalX] = spriteToDraw->bitmap[i];
    }
}
