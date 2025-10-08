#include <stdlib.h>
#include "power_up.h"

#define WIDTH 320
#define HEIGHT 240

// Pseudo-random number generator
static uint32_t rng_state = 12345;

static void simple_srand(uint32_t seed)
{
    rng_state = seed;
}

static uint32_t simple_rand(void)
{
    // Linear congruential generator
    rng_state = rng_state * 1103515245 + 12345;
    return (rng_state / 65536) % 32768;
}


PowerUp spawn_power_up(int power_type, int frame_counter)
{
    YBoundary y_bounds = {.top = HEIGHT - 40, .bottom = HEIGHT - 200};
    XBoundary x_bounds = {.left = WIDTH - 240, .right = WIDTH - 80};

    PowerUp p;

    simple_srand(frame_counter * 7919);

    p.x = x_bounds.left + simple_rand() % (x_bounds.right - x_bounds.left + 1);
    p.y = y_bounds.bottom + simple_rand() % (y_bounds.top - y_bounds.bottom + 1);
    p.type = power_type;

    power_up_position(&p.x, &p.y, &p.type);
    draw_powerup(&p.x, &p.y, &p.type);

    return p;
}

PowerUp rand_power_up(void)
{
    int power_type = (simple_rand() % 3) + 1;
    return spawn_power_up(power_type, 1);
}

/**
 * Checks which player was the last to hit the ball with their paddle
 * Checks if ball == power_up_position
 */
void power_up_position(int *px, int *py, int *p_type)
{
    while (1)
    {
        int curr_player = player_ball();

        /*if (bx < px + p_sz &&
            bx + ball_sz > px &&
            by < py + p_sz &&
            by + ball_sz > py)*/
        // activates power_up by calling on the appropiate function
        switch (*p_type)
        {
        case 1:
            bigPaddle_power_up();
            break;
        case 2:
            speedUp_power_up();
            break;
        case 3:
            doubleBall_power_up();
            break;
        }
    }
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