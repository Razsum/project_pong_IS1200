#include "dtekv-lib.h"
#include "dtekv-mpu6050-lib/dtekv-mpu6050-lib.h"
#include "dtekv-i2c-lib/dtekv-i2c-lib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "objects.h"
#include "simple-math.h"
#include "graphics.h"

#define SENSITIVITY 180
#define MAX_SCORE 10
#define PADDLE_VEL 1
#define BALL_VEL 2

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

int main()
{
  int p1_score = 0, p2_score = 0;
  
  initializeSensor(0);
  initializeSensor(1);

  clear_screen8(COL_BG);

  short x1 = 0;
  short y1 = 0;
  short x2 = 0;
  short y2 = 0;

  initialize_ball(BALL_VEL);

  while (p1_score < MAX_SCORE && p2_score < MAX_SCORE)
  {
    getAccelerometer(0, &x1, &y1);
    getAccelerometer(1, &x2, &y2);
    
    
    y1 = y1 / SENSITIVITY;
    y2 = y2 / SENSITIVITY;
    
    // Player 1 controls
    if (y1 < -20) {          // Tilted one way
        d1y = -PADDLE_VEL;            // Move up
    } else if (y1 > 20) {    // Tilted other way  
        d1y = PADDLE_VEL;             // Move down
    } else {
        d1y = 0;
    }
    
    // Player 2 controls
    if (y2 < -20) {          // Tilted one way
        d2y = -PADDLE_VEL;            // Move up
    } else if (y2 > 20) {    // Tilted other way  
        d2y = PADDLE_VEL;             // Move down
    } else {
      d2y = 0;
    }

    prints(y2);
    
    // ---- UPDATE ----
    update_ball_physics(&p1_score, &p2_score);
    update_player_position();
    draw_all(p1_score, p2_score);

    wait(5);
  }

  if (p1_score == MAX_SCORE) {
      print("Player 1 wins!");
  }
  if (p2_score == MAX_SCORE) {
    print("Player 2 wins!");
  }
}
