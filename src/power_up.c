#include <stdlib.h>
#include "power_up.h"

#define WIDTH 320
#define HEIGHT 240

const int p_sz = 5;

static uint32_t frame_counter = 0;

enum
{
  COL_BALL = 0xC0 // ball
};

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

const int ball_sz = 5;
float bx = WIDTH / 2 - ball_sz / 2.0f, by = HEIGHT / 2 - ball_sz / 2.0f;


PowerUp spawn_power_up(int power_type)
{
    YBoundary y_bounds = {.top = HEIGHT - 40, .bottom = HEIGHT - 200};
    XBoundary x_bounds = {.left = WIDTH - 240, .right = WIDTH - 80};

    PowerUp p;

    simple_srand(frame_counter * 7919);

    p.x = x_bounds.left + simple_rand() % (x_bounds.right - x_bounds.left + 1);
    p.y = y_bounds.bottom + simple_rand() % (y_bounds.top - y_bounds.bottom + 1);
    p.type = power_type;

    p.sprite = get_sprite(power_type - 1);
    power_up_position(&p.x, &p.y, &p.type);
    return p;
}

PowerUp rand_power_up(void)
{
    int power_type = (simple_rand() % 3) + 1;
    return spawn_power_up(power_type);
}

/**
 * Checks which player was the last to hit the ball with their paddle
 * Checks if ball == power_up_position
 */
void power_up_position(int *px, int *py, int *p_type)
{
    while(1) {
    // will only be active when power up has spawned
    rect_fill8(px, py, p_sz, p_sz, COL_BALL);
    int curr_player = player_ball();

    /*if (bx < px + p_sz &&
        bx + ball_sz > px &&
        by < py + p_sz &&
        by + ball_sz > py)*/
    // activates power_up by calling on the appropiate function
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