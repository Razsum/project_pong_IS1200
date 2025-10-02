#ifndef _DTEKV_I2C_LIB_H_
#define _DTEKV_I2C_LIB_H_

#define GPIO_ADDRESS 0x40000e0
#define GPIO_DIRECTION_ADDRESS 0x40000e4
#define SCL_PIN 0
#define SDA_PIN 1
#define DELAY_NOPS 10

void requestFrom(unsigned char address, unsigned char quantity, unsigned char *buffer);
void beginTransmission(unsigned char address);
unsigned char write(unsigned char byte);
void endTransmission(unsigned char done);

#endif