#include "dtekv-lib.h"
#include "dtekv-mpu6050-lib/dtekv-mpu6050-lib.h"
#include "dtekv-i2c-lib/dtekv-i2c-lib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "objects.h"
#include "simple-math.h"
#include "graphics.h"

/**
 * @brief Timer addresses
 *
 * The timer is memory-mapped, starting at address 0x04000020.
 * All timer registers are defined relative to the base address.
 * The macros define the offsets for the timer's control, status, and period registers
 */
#define TIMER_BASE 0x04000020u
#define TMR_STATUS (*(volatile uint32_t *)(TIMER_BASE + 0x00))
#define TMR_CONTROL (*(volatile uint32_t *)(TIMER_BASE + 0x04))
#define TMR_PERIODL (*(volatile uint32_t *)(TIMER_BASE + 0x08))
#define TMR_PERIODH (*(volatile uint32_t *)(TIMER_BASE + 0x0C))

/**
 * @brief Timer control bit masks
 *
 * Configurse timer behaviour through the control register.
 * The macros define the bitmasks
 */
#define CTRL_ITO (1u << 0)   // Interrupts on timeout enable
#define CTRL_CONT (1u << 1)  // Continious mode enable
#define CTRL_START (1u << 2) // Start timer
#define CTRL_STOP (1u << 3)  // Stop timer

/**
 * @brief Timer status bit masks
 *
 * Used to check or clear timer status flags
 */
#define STAT_TO (1u << 0)  // Timeout flag
#define STAT_RUN (1u << 1) // Timer running flag

/**
 * @brief Button input address
 *
 * Memory-mapped address of the KEY1 button.
 * Used for starting and resetting the game.
 */
#define BTN_ADDR ((volatile uint32_t *)0x040000d0u)

/**
 * @brief Game constants
 *
 */
#define SENSITIVITY 180 // Accelerometer sensitivity
#define MAX_SCORE 10    // Winning condition
#define PADDLE_VEL 1    // Paddle movement speed
#define BALL_VEL 2      // Initial ball speed

/**
 * @brief Game state enumeration
 *
 * Represents the three main states of the game
 *
 * @author Kai Nguyen
 */
typedef enum
{
    STATE_MENU,     // The game is waiting to start
    STATE_PLAYING,  // The game is currently playing
    STATE_GAME_OVER // The game is over / victory screen
} GameState;

/**
 * @brief Player scores
 *
 */
int p1_score = 0, p2_score = 0;

/**
 * @brief Global game state variable
 *
 */
volatile GameState game_state = STATE_MENU;

/**
 * @brief Button press
 *
 * Set to true when button press is detected
 *
 */
volatile bool button_pressed = false;

/**
 * @brief Initializes the hardware timer
 *
 * The timer is enabled to generate periodic interrupts,
 * which drives the game loop updates. The timer is set to continuous
 * mode with interrupt enabled
 * @author Kai Nguyen
 */
void initialize_tmr(void)
{
    TMR_PERIODL = 0x4F90;
    TMR_PERIODH = 0x0002;
    TMR_STATUS = STAT_TO;
    TMR_CONTROL = CTRL_CONT | CTRL_START | CTRL_ITO;
    enable_interrupt();
}

/**
 * @brief Reads the button input state
 *
 * @return int Returns 1 if the button is pressed, 0 otherwise.
 * @author Kai Nguyen
 */
int get_btn(void)
{
    return (int)(*BTN_ADDR & 0x1u);
}

/**
 * @brief Interrupt handler for timer events
 *
 * Handles updates triggered by the timer interrupt:
 * - Clears the timer interrupt flag
 * - Checks for button presses
 * - Update the game physics
 * - Redraws graphics
 * - Handle any possible state transitions
 * @param cause The interrupt cause
 * @author Kai Nguyen
 */
void handle_interrupt(unsigned cause)
{
    TMR_STATUS = STAT_TO;

    // Detect button press
    if (get_btn())
    {
        button_pressed = true;
    }

    // Update game logic only during active play
    if (game_state == STATE_PLAYING)
    {
        if (p1_score < MAX_SCORE && p2_score < MAX_SCORE)
        {
            update_ball_physics(&p1_score, &p2_score);
            update_player_position();
            draw_all(p1_score, p2_score);

            // Checks for game over
            if (p1_score >= MAX_SCORE || p2_score >= MAX_SCORE)
            {
                game_state = STATE_GAME_OVER;
                reset_player_position();
                reset_ball_position();
            }
        }
    }
}

/**
 * @brief Main game loop
 *
 * Initializes sensors, the timer, and the display, then enters
 * the main game loop which handles startmenu, gameplay, 
 * and game states
 *
 * The loop continuously:
 * - Displays the start menu waiting for button input to start game
 * - Reads accelerometer data for paddle movement
 * - Updates scores and detects winners
 * - Draws updated frames to the framebuffer
 * @return int Never returns (infinite loop)
 * @author Kai Nguyen
 */
int main()
{
    initializeSensor(0);
    initializeSensor(1);
    initialize_tmr();

    clear_screen8(COL_BG);
    draw_all(p1_score, p2_score);

    button_pressed = false;

    int text_x = (WIDTH - 84) / 2;
    int text_y = HEIGHT / 2 - 3;

    short x1 = 0, y1 = 0;
    short x2 = 0, y2 = 0;

    while (1)
    {
        switch (game_state)
        {
        case STATE_MENU:
            draw_text(text_x - 10, text_y + 20, "Press KEY1 to start", COL_GOLD);

            if (button_pressed)
            {
                button_pressed = false;

                // Clears text and starts game by initialising and drawing graphics
                rect_fill8(text_x - 10, text_y + 20, 19 * 6, 7, COL_BG);
                p1_score = 0;
                p2_score = 0;
                reset_player_position();
                reset_ball_position();
                initialize_ball(BALL_VEL);
                draw_all(p1_score, p2_score);

                game_state = STATE_PLAYING;
            }
            break;

        case STATE_PLAYING:
            // Read accelerometer input
            getAccelerometer(0, &x1, &y1);
            getAccelerometer(1, &x2, &y2);

            y1 = y1 / SENSITIVITY;
            y2 = y2 / SENSITIVITY;

            // Player 1 controls
            if (y1 < -10)
            {
                d1y = -PADDLE_VEL;
            }
            else if (y1 > 10)
            {
                d1y = PADDLE_VEL;
            }
            else
            {
                d1y = 0;
            }

            // Player 2 controls
            if (y2 < -10)
            {
                d2y = -PADDLE_VEL;
            }
            else if (y2 > 10)
            {
                d2y = PADDLE_VEL;
            }
            else
            {
                d2y = 0;
            }

            // Check for button press to quit
            if (button_pressed)
            {
                button_pressed = false;
                game_state = STATE_GAME_OVER;
                reset_player_position();
                reset_ball_position();
            }
            break;

        case STATE_GAME_OVER:
            if (p1_score >= MAX_SCORE)
            {
                draw_text(text_x, text_y, "Player 1 wins", COL_GOLD);
            }
            else if (p2_score >= MAX_SCORE)
            {
                draw_text(text_x, text_y, "Player 2 wins", COL_GOLD);
            }

            // Wait for button to return to menu
            if (button_pressed)
            {
                button_pressed = false;

                // Clear winner text
                rect_fill8(text_x, text_y, 13 * 6, 7, COL_BG);
                clear_screen8(COL_BG);
                draw_all(p1_score, p2_score);
                game_state = STATE_MENU;
            }
            break;
        }
    }
}
