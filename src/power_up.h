#ifndef POWER_UP_H
#define POWER_UP_H

#include <stdint.h>

typedef struct {
    int top;
    int bottom;
} YBoundary;

typedef struct {
    int left;
    int right;
} XBoundary;

typedef struct {
    int x;
    int y;
    int type;
} PowerUp;

PowerUp spawn_power_up(int power_type);
PowerUp rand_power_up();
void power_up_position(int *power_x, int *power_y, int *power_type, int current_player);
void bigPaddle_power_up(int curr_player);
void speedUp_power_up(int curr_player);
void doubleBall_power_up(int curr_player);

#endif