#include "dtekv-lib.h"
#include "dtekv-mpu6050-lib/dtekv-mpu6050-lib.h"
#include "power_up.h"
#include <stdint.h>
#include <stdbool.h>
#include "sprites.h"

#define SENSITIVITY 180
#define M_PI 3.14159265358979323846
#define FB_BASE 0x08000000u

#define WIDTH 320
#define HEIGHT 240

#define GPIO_IN_ADDR 0x040000E0u
#define GPIO_IN (*(volatile uint32_t *)GPIO_IN_ADDR)

enum
{
    COL_NET = 0x80, // mid gray (center net)
};

/**
 * Angel approximation to calculate ball trajectory
 */
// Cosine approximation
static float simple_cos(float x)
{
    // Taylor series: cos(x) ≈ 1 - x²/2 + x⁴/24
    float x2 = x * x;
    return 1.0f - x2 * 0.5f + x2 * x2 * 0.041666f;
}
// Sinus approximation
static float simple_sin(float x)
{
    // Taylor series: sin(x) ≈ x - x³/6 + x⁵/120
    float x2 = x * x;
    return x * (1.0f - x2 * 0.166666f + x2 * x2 * 0.008333f);
}

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

// Ball object
const int ballSize = 8;
const int ballVelocity = 2;
int ballX, ballY;
int ballPrevX = 1, ballPrevY = 1;
float ballDX = 1, ballDY = 1;

// Paddle object
const int paddleWidth = 6, paddleHeight = 50;
int p1x, p1y;
int p2x, p2y;
int p1PrevX = 10, p1PrevY = HEIGHT / 2 - paddleHeight / 2;
int p2PrevX = WIDTH - 16, p2PrevY = HEIGHT / 2 - paddleHeight / 2;

/* 8-bit framebuffer pointer (1 byte per pixel) */
static volatile uint8_t *const fb = (volatile uint8_t *)FB_BASE;
void render_sprite(int x, int y, int spriteID);
static uint32_t frameCounter = 0;

static void clear_screen(uint8_t c)
{
    for (int i = 0; i < WIDTH * HEIGHT; ++i)
        fb[i] = c;
}

void ball_spawn()
{
    ballX = WIDTH / 2 - 16;
    ballY = HEIGHT / 2 - 16;
    render_sprite(ballX, ballY, 4);

    simple_srand(frameCounter * 7919);

    int randomX = simple_rand() % 2;
    int randomY = simple_rand() % 2;

    if (randomX)
    {
        ballDX = -ballDX;
    }

    if (randomY)
    {
        ballDY = -ballDY;
    }
}

void paddle_spawn_p1()
{
    p1x = 10;
    p1y = HEIGHT / 2;
    render_sprite(p1x, p1y, 3);
}

void paddle_spawn_p2()
{
    p2x = WIDTH - 10;
    p2y = HEIGHT / 2;
    render_sprite(p2x, p2y, 3);
}

static void rect_fill8(int x, int y, int w, int h, uint8_t c)
{
    if (w <= 0 || h <= 0)
        return;
    if (x < 0)
    {
        w += x;
        x = 0;
    }
    if (y < 0)
    {
        h += y;
        y = 0;
    }
    if (x + w > WIDTH)
        w = WIDTH - x;
    if (y + h > HEIGHT)
        h = HEIGHT - y;

    for (int j = 0; j < h; ++j)
    {
        volatile uint8_t *row = fb + (y + j) * WIDTH + x;
        for (int i = 0; i < w; ++i)
            row[i] = c;
    }
}

void draw_net()
{
    for (int y = 0; y < HEIGHT; y += 8)
        rect_fill8(WIDTH / 2 - 1, y, 2, 4, COL_NET);
}

void ball_movement()
{
    // erase old ball
    for (int j = 0; j < 32; j++)
        for (int i = 0; i < 32; i++)
        {
            int px = ballX + i;
            int py = ballY + j;
            if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT)
                fb[py * WIDTH + px] = FB_BASE;
        }

    // update position
    ballX += ballDX;
    ballY += ballDY;

    // bounce off edges
    if (ballX < 0 || ballX + 32 > WIDTH)
        ballDX = -ballDX;
    if (ballY < 0 || ballY + 32 > HEIGHT)
        ballDY = -ballDY;

    // redraw ball
    render_sprite(ballX, ballY, 4);
}
static void update_ball_physics(int *p1Score, int *p2Score)
{
    ballPrevX = ballX;
    ballPrevY = ballY;
    ballX += (int)ballDX;
    ballY += (int)ballDY;

    // Ball hits left paddle
    if (ballX <= p1x + paddleWidth && (ballY + ballSize >= p1y && ballY <= p1y + paddleHeight) && ballDX < 0)
    {
        ballX = p1x + paddleWidth;
        float hit_position = (ballY - (p1y + paddleHeight / 2.0f)) / (paddleHeight / 2.0f);

        if (hit_position < -1.0f)
            hit_position = -1.0f;
        if (hit_position > 1.0f)
            hit_position = 1.0f;

        float max_bounce_angle = M_PI / 4.0f;
        float bounce_angle = hit_position * max_bounce_angle;
        ballDX = ballVelocity * simple_cos(bounce_angle);
        ballDY = ballVelocity * simple_sin(bounce_angle);
    }

    // Ball hits right paddle
    if (ballX + ballSize >= p2x && (ballY + ballSize >= p2y && ballY <= p2y + paddleHeight) && ballDX > 0)
    {
        ballX = p2x - ballSize;
        float hit_position = (ballY - (p2y + paddleHeight / 2.0f)) / (paddleHeight / 2.0f);

        if (hit_position < -1.0f)
            hit_position = -1.0f;
        if (hit_position > 1.0f)
            hit_position = 1.0f;

        float max_bounce_angle = M_PI / 4.0f;
        float bounce_angle = hit_position * max_bounce_angle;
        ballDX = -ballVelocity * simple_cos(bounce_angle);
        ballDY = ballVelocity * simple_sin(bounce_angle);
    }

    // Ball hits wall
    if (ballY <= 0 || ballY + ballSize >= HEIGHT)
    {
        ballDY = -ballDY;
        if (ballY <= 0)
            ballY = 0;
        if (ballY + ballSize >= HEIGHT)
            ballY = HEIGHT - ballSize;
    }

    if (ballX <= 0)
    {
        (*p2Score)++;
        ballX = WIDTH / 2 - ballSize / 2;
        ballY = HEIGHT / 2 - ballSize / 2;
        ballDX = ballVelocity;
        ballDY = 0.0f;
        frameCounter++;
        ball_spawn();
    }

    if (ballX >= WIDTH)
    {
        (*p1Score)++;
        ballDX = -ballVelocity;
        ballDY = 0.0f;
        frameCounter++;
        ball_spawn();
    }
}
static void update_player_position(int d1y, int d2y)
{
  p1PrevX = p1x;
  p1PrevY = p1y;
  p2PrevX = p2x;
  p2PrevY = p2y;

  p1y += d1y;
  p2y += d2y;
  if (p1y < 0)
    p1y = 0;
  if (p1y + paddleHeight > HEIGHT)
    p1y = HEIGHT - paddleHeight;

  if (p2y < 0)
    p2y = 0;
  if (p2y + paddleHeight > HEIGHT)
    p2y = HEIGHT - paddleHeight;
}

static void wait(unsigned short ms)
{
    for (unsigned int i = 0; i < 10000 * ms; i++)
        asm volatile("nop");
}

static void prints(short s)
{
    if (s < 0)
    {
        printc('-');
        print_dec(-s);
    }
    else
        print_dec(s);
}

void handle_interrupt(void) {}

int main(void)
{
    int p1Score = 0, p2Score = 0;

    initializeSensor(0);
    initializeSensor(1);

    clear_screen(FB_BASE);

    draw_net();
    paddle_spawn_p1();
    paddle_spawn_p2();

    ball_spawn();

    short x1, y1 = 0;
    short x2, y2 = 0;

    rand_power_up();

    while (p1Score < 10 && p2Score < 10)
    {
        getAccelerometer(0, &x1, &y1);
        getAccelerometer(1, &x2, &y2);
        y1 = y1 / SENSITIVITY;
        y2 = y2 / SENSITIVITY;
        int d1y = 0;
        int d2y = 0;

        // Player 1 controls
        if (y1 < -10)
        {
            d1y = -1;
        }
        else if (y1 > 10)
        {
            d1y = 1;
        }

        // Player 2 controls
        if (y2 < -10)
        {
            d2y = -1;
        }
        else if (y2 > 10)
        {
            d2y = 1;
        }

        print(" Y1=");
        prints(y1);
        print(" Y2=");
        prints(y2);
        print("\n");

        // ---- UPDATE ----
        update_ball_physics(&p1Score, &p2Score);
        update_paddle_position(d1y, d2y);
        draw_all(p1Score, p2Score);

        wait(5);

        rand_power_up();
    }

    if (p1Score == 10)
    {
        print("Player 1 wins!");
    }
    if (p2Score == 10)
    {
        print("Player 2 wins!");
    }
}
