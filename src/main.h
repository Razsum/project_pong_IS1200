#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "objects.h"

// Frame counter
extern uint32_t frame_counter;

void wait(unsigned short ms);

void clear_screen8(uint8_t c);

void rect_fill8(int x, int y, int w, int h, uint8_t c);
void pset8(int x, int y, uint8_t c);

void draw_digit(int x, int y, int digit, uint8_t color);
void draw_score(int x, int y, int score, uint8_t color);
void draw_all(int p1_score, int p2_score);
void draw_powerup(int *px, int *py, int *p_type);

void initialize_ball(void);

int player_ball(int n);

void update_ball_physics(int *p1_score, int *p2_score);
void update_player_position(int d1y, int d2y);

void prints(short s);

#endif // MAIN_H
