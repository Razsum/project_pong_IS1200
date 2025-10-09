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
#define TMR_STATUS   (*(volatile uint32_t*)(TIMER_BASE + 0x00)) // bit0=TO, bit1=RUN
#define TMR_CONTROL  (*(volatile uint32_t*)(TIMER_BASE + 0x04)) // ITO, CONT, START, STOP
#define TMR_PERIODL  (*(volatile uint32_t*)(TIMER_BASE + 0x08)) // low 16 bits
#define TMR_PERIODH  (*(volatile uint32_t*)(TIMER_BASE + 0x0C)) // high 16 


// control bits per doc
#define CTRL_ITO     (1u << 0)  // enable IRQ (not needed for polling)
#define CTRL_CONT    (1u << 1)  // continuous mode
#define CTRL_START   (1u << 2)  // write-1 event
#define CTRL_STOP    (1u << 3)  // write-1 event

// status bits
#define STAT_TO      (1u << 0)  // timeout flag
#define STAT_RUN     (1u << 1)  // running (read-only)

#define SENSITIVITY 180
#define MAX_SCORE 10
#define PADDLE_VEL 1
#define BALL_VEL 2

int p1_score = 0, p2_score = 0;

void initialize_tmr(void)
{
    TMR_PERIODL = 0xC6C0;
    TMR_PERIODH = 0x002D;
    
    TMR_STATUS = STAT_TO;
    
    TMR_CONTROL = CTRL_CONT | CTRL_START | CTRL_ITO;
    
    enable_interrupt();
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

void handle_interrupt(unsigned cause) 
{
	TMR_STATUS = STAT_TO;
	update_ball_physics(&p1_score, &p2_score);
  update_player_position();
  draw_all(p1_score, p2_score);
}


int main()
{ 
  initializeSensor(0);
  initializeSensor(1);
  initialize_tmr();

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
    if (y1 < -10) {          // Tilted one way
        d1y = -PADDLE_VEL;            // Move up
    } else if (y1 > 10) {    // Tilted other way  
        d1y = PADDLE_VEL;             // Move down
    } else {
        d1y = 0;
    }
    
    // Player 2 controls
    if (y2 < -10) {          // Tilted one way
        d2y = -PADDLE_VEL;            // Move up
    } else if (y2 > 10) {    // Tilted other way  
        d2y = PADDLE_VEL;             // Move down
    } else {
      d2y = 0;
    }
  }

  if (p1_score == MAX_SCORE) {
      print("Player 1 wins!");
  }
  if (p2_score == MAX_SCORE) {
    print("Player 2 wins!");
  }
}
