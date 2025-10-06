#include <stdlib.h>
#include <time.h>
#include "power_up.h"
#include "sprites.h"

#define WIDTH 320
#define HEIGHT 240

PowerUp spawn_power_up(int power_type)
{
    YBoundary y_bounds = {.top = HEIGHT - 40, .bottom = HEIGHT - 200};
    XBoundary x_bounds = {.left = WIDTH - 240, .right = WIDTH - 80};

    PowerUp p;

    p.x = x_bounds.left + rand() % (x_bounds.right - x_bounds.left + 1);
    p.y = y_bounds.bottom + rand() % (y_bounds.top - y_bounds.bottom + 1);
    p.type = power_type;

    return p;
}

PowerUp rand_power_up(void)
{
    int power_type = (rand() % 3) + 1; // random number between 1 and 3
    switch (power_type)
    {
    case 1:
        power_type = 1;
        break;
    case 2:
        power_type = 2;
        break;
    case 3:
        power_type = 3;
        break;
    default:
        break;
    }

    spawn_power_up(power_type);
}

PowerUp goalkeeper_power_up(void)
{
}

PowerUp speedUp_power_up(void)
{
}

PowerUp doubleBall_power_up(void)
{
}