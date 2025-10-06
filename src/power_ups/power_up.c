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

    p.sprite = get_sprite(power_type - 1);
    return p;
}

PowerUp rand_power_up(void)
{
    int power_type = (rand() % 3) + 1;
    return spawn_power_up(power_type);
}

/**
 * Sets up two "goal posts" for the player that acquires the power up,
 * lasts for 'x' clock cycles
 */
PowerUp goalKeeper_power_up(void)
{

}

/**
 * Speeds up the velocity of the ball when the player that acquires it
 * hits it with their paddle, lasts for 'x' clock cycles
 */
PowerUp speedUp_power_up(void)
{

}
/**
 * Spawns a second ball that will move at slower speed towards the
 * opponent of the player that acquired it, will despawn when it hits
 * opponent's paddle or ball
 */
PowerUp doubleBall_power_up(void)
{

}