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
    power_up_position(&p.x, &p.y, &p.type);
    return p;
}

PowerUp rand_power_up(void)
{
    int power_type = (rand() % 3) + 1;
    return spawn_power_up(power_type);
}

/**
 * Checks which player was the last to hit the ball with their paddle
 * Checks if ball == power_up_position
 */
void power_up_position(PowerUp *px, PowerUp *py, PowerUp *ptype)
{
    // will only be active when power up has spawned
    // get p.x and p.y
    // Check which player was last to hit the paddle
    // new function (hit_ball), updated by update_ball_physics
    //
    // Checks ball position, if bx == p.x && by == p.y (+ marginal)
    // activates power_up by calling on the appropiate function
}

/**
 * Increase the length of the paddle for the player that acquires it
 * Despawns after 'x' paddle hits
 */
void bigPaddle_power_up(void)
{
    // p(num) update_paddle_position
    // p(num) registers new bitmap as paddle
    // Inactivates after 'x' paddle hits
}

/**
 * Speeds up the velocity of the ball when the player that acquires it
 * hits it with their paddle, lasts for 'x' paddle hits
 */
void speedUp_power_up(void)
{
    // Only active for the player that accumulates it
    // if p(num)x register hits on paddle
    // ball_vel = ball_vel + 0.5
    // if p(opp_num) register hit on paddle
    // ball_vel = ball_vel -0.5
    // Inactivates after 'x' paddle hits
}
/**
 * Spawns a second ball that will move at slower speed towards the
 * opponent of the player that acquired it, will despawn when it hits
 * opponent's paddle or ball
 */
void doubleBall_power_up(void)
{
    // intialise_ball (second ball)
    // ball_vel = ball_vel - 1
    // update_ball_physcics
    // Send it towards p(opp_num) side
    // Despawn (erase) ball once it hits p(opp_num) paddle
    // Despawn (and score+1) if hit p(opp_num) side
}