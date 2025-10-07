#include "dtekv-lib.h"
#include "dtekv-mpu6050-lib/dtekv-mpu6050-lib.h"

#define SENSITIVITY 180

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
    initializeSensor(0);

    short x = 0;
    short y = 0;

    while (1)
    {
        getAccelerometer(0, &x, &y);

        print("X: ");
        prints(x / SENSITIVITY);
        print(" Y: ");
        prints(y / SENSITIVITY);
        print("\n");

        wait(500);
    }
}