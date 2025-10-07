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
    const void* sprite;
} PowerUp;

PowerUp spawn_power_up(int power_type);
PowerUp rand_power_up(void);

#endif