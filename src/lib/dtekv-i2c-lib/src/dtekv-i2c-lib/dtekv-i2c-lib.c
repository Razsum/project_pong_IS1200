#include "dtekv-i2c-lib/dtekv-i2c-lib.h"

#define SDA_ON set_pin(SDA_PIN)
#define SDA_OFF clear_pin(SDA_PIN)
#define SDA_READ read_pin(SDA_PIN)

#define SCL_ON set_pin(SCL_PIN)
#define SCL_OFF clear_pin(SCL_PIN)
#define SCL_READ read_pin(SCL_PIN)

#define TRUE 1
#define FALSE 0

#define READ 0b00000001
#define WRITE 0b00000000

#define BITS_PER_BYTE 8

#define DELAY delay(DELAY_NOPS)

#define GPIO (volatile int *)GPIO_ADDRESS
#define GPIO_DIRECTION (volatile int *)GPIO_DIRECTION_ADDRESS

static unsigned char isTransmitting = FALSE;

static inline void delay(unsigned int nop)
{
    for (unsigned int i = 0; i < nop; i++)
        asm volatile("nop");
}

static inline void clear_pin(unsigned int pin)
{
    *GPIO_DIRECTION |= 1 << pin;
    *GPIO &= ~(1 << pin);
}

static inline void set_pin(unsigned int pin)
{
    *GPIO_DIRECTION |= 1 << pin;
    *GPIO |= 1 << pin;
}

static inline unsigned char read_pin(unsigned int pin)
{
    *GPIO_DIRECTION &= ~(1 << pin);
    return (*GPIO & (1 << pin)) >> pin;
}

static void start()
{
    SDA_ON;
    DELAY;
    SCL_ON;
    DELAY;
    SDA_OFF;
    DELAY;
    SCL_OFF;
    DELAY;
}

static void stop()
{
    SDA_OFF;
    DELAY;
    SCL_ON;
    DELAY;
    SDA_ON;
    DELAY;
}

static unsigned char Tx(unsigned char data)
{

    for (unsigned char bit = BITS_PER_BYTE; bit > 0; bit--)
    {
        (data & (1 << (bit - 1))) ? SDA_ON : SDA_OFF;
        DELAY;
        SCL_ON;
        DELAY;
        SCL_OFF;
        DELAY;
    }

    SDA_ON;
    DELAY;
    SCL_ON;
    DELAY;
    unsigned char ack = !SDA_READ;
    SCL_OFF;

    return ack;
}

static unsigned char Rx(unsigned char ack)
{
    SDA_ON;

    unsigned char data = 0;
    for (unsigned char bit = BITS_PER_BYTE; bit > 0; bit--)
    {
        // clock stretching
        do
        {
            SCL_ON;
        } while (SCL_READ == 0);

        DELAY;
        if (SDA_READ)
            data |= (1 << (bit - 1));
        DELAY;
        SCL_OFF;
    }

    DELAY;
    ack ? SDA_OFF : SDA_ON;
    DELAY;
    SCL_ON;
    DELAY;
    SCL_OFF;
    SDA_ON;

    return data;
}

void requestFrom(unsigned char address, unsigned char quantity, unsigned char *buffer)
{
    Tx((address << 1) | READ);

    for (unsigned char i = 0; i < (quantity - 1); i++)
        buffer[i] = Rx(1);

    buffer[quantity - 1] = Rx(0);
}

void beginTransmission(unsigned char address)
{
    if (!isTransmitting)
    {
        isTransmitting = TRUE;
        start();
    }

    Tx((address << 1) | WRITE);
}

unsigned char write(unsigned char byte)
{
    return Tx(byte);
}

void endTransmission(unsigned char done)
{
    if (!done)
    {
        start();
        return;
    }

    isTransmitting = FALSE;
    stop();
}
