#include "dtekv-lib.h"
#include "dtekv-mpu6050-lib/dtekv-mpu6050-lib.h"
#include "dtekv-i2c-lib/dtekv-i2c-lib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "objects.h"
#include "simple-math.h"
#include "graphics.h"

#define TIMER_BASE   0x04000020u
#define TMR_STATUS   (*(volatile uint32_t*)(TIMER_BASE + 0x00))
#define TMR_CONTROL  (*(volatile uint32_t*)(TIMER_BASE + 0x04))
#define TMR_PERIODL  (*(volatile uint32_t*)(TIMER_BASE + 0x08))
#define TMR_PERIODH  (*(volatile uint32_t*)(TIMER_BASE + 0x0C))

#define CTRL_ITO     (1u << 0)
#define CTRL_CONT    (1u << 1)
#define CTRL_START   (1u << 2)
#define CTRL_STOP    (1u << 3)

#define STAT_TO      (1u << 0)
#define STAT_RUN     (1u << 1)

#define BTN_ADDR ((volatile uint32_t*)0x040000d0u)

#define SENSITIVITY 180
#define MAX_SCORE 10
#define PADDLE_VEL 1
#define BALL_VEL 2

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAME_OVER
} GameState;

int p1_score = 0, p2_score = 0;
volatile GameState game_state = STATE_MENU;
volatile bool button_pressed = false;

void initialize_tmr(void)
{
    TMR_PERIODL = 0x4F90;
    TMR_PERIODH = 0x0002;
    TMR_STATUS = STAT_TO;
    TMR_CONTROL = CTRL_CONT | CTRL_START | CTRL_ITO;
    enable_interrupt();
}

int get_btn(void) {
    return (int)(*BTN_ADDR & 0x1u);
}

void handle_interrupt(unsigned cause) 
{
    TMR_STATUS = STAT_TO;

    // Detect button press
    if (get_btn()) {
        button_pressed = true;
    }

    // Update game physics only when playing
    if (game_state == STATE_PLAYING) {
        if (p1_score < MAX_SCORE && p2_score < MAX_SCORE) {
            update_ball_physics(&p1_score, &p2_score);
            update_player_position();
            draw_all(p1_score, p2_score);
            
            // Check for game over
            if (p1_score >= MAX_SCORE || p2_score >= MAX_SCORE) {
                game_state = STATE_GAME_OVER;
                reset_player_position();
                reset_ball_position();
            }
        }
    }
}

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

    while (1) {
        switch (game_state) {
            case STATE_MENU:
                // Draw start message
                draw_text(text_x - 10, text_y + 20, "Press KEY1 to start", COL_GOLD);
                
                // Wait for button press
                if (button_pressed) {
                    button_pressed = false;
                    
                    // Clear text and start game
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
                if (y1 < -10) {
                    d1y = -PADDLE_VEL;
                } else if (y1 > 10) {
                    d1y = PADDLE_VEL;
                } else {
                    d1y = 0;
                }
                
                // Player 2 controls
                if (y2 < -10) {
                    d2y = -PADDLE_VEL;
                } else if (y2 > 10) {
                    d2y = PADDLE_VEL;
                } else {
                    d2y = 0;
                }

                // Check for button press to quit
                if (button_pressed) {
                    button_pressed = false;
                    game_state = STATE_GAME_OVER;
                    reset_player_position();
                    reset_ball_position();
                }
                break;

            case STATE_GAME_OVER:
                // Display winner
                if (p1_score >= MAX_SCORE) {
                    draw_text(text_x, text_y, "Player 1 wins", COL_GOLD);
                } else if (p2_score >= MAX_SCORE) {
                    draw_text(text_x, text_y, "Player 2 wins", COL_GOLD);
                }
                
                // Wait for button to return to menu
                if (button_pressed) {
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
