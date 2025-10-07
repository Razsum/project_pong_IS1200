#include "dtekv-mpu6050-lib/dtekv-mpu6050-lib.h"
#include "dtekv-i2c-lib/dtekv-i2c-lib.h"

#define MPU6050_RA_ACCEL_XOUT_H 0x3B
#define MPU6050_RA_ACCEL_XOUT_L 0x3C
#define MPU6050_RA_ACCEL_YOUT_H 0x3D
#define MPU6050_RA_ACCEL_YOUT_L 0x3E

#define MPU6050_RA_PWR_MGMT_1 0x6B
#define MPU6050_RA_SIGNAL_PATH_RESET 0x68

#define DEVICE_RESET 0b10000000
#define TEMP_DIS 0b00001000

#define GYRO_RESET 0b00000100
#define ACCEL_RESET 0b00000010
#define TEMP_RESET 0b00000001

static inline void wait(unsigned char ms)
{
    for (unsigned int i = 0; i < NOPS_PER_MS * ms; i++)
        asm volatile("nop");
}

void initializeSensor(int sensor)
{
    unsigned char address = MPU6050_ADDRESS + sensor; // sensor 0: 0x68, sensor 1: 0x69
    
    // Device Reset
    beginTransmission(address);
    write(MPU6050_RA_PWR_MGMT_1);
    write(0 | DEVICE_RESET | TEMP_DIS);
    endTransmission(1);

    wait(100);

    // Signal Path Reset
    beginTransmission(address);
    write(MPU6050_RA_SIGNAL_PATH_RESET);
    write(0 | GYRO_RESET | ACCEL_RESET | TEMP_RESET);
    endTransmission(1);

    wait(100);

    // Wake up
    beginTransmission(address);
    write(MPU6050_RA_PWR_MGMT_1);
    write(0 | TEMP_DIS);
    endTransmission(1);
}

void getAccelerometerX(short *x)
{
    unsigned char buffer[2];

    beginTransmission(MPU6050_ADDRESS);
    write(MPU6050_RA_ACCEL_XOUT_H);
    endTransmission(0);

    requestFrom(MPU6050_ADDRESS, 2, buffer);
    endTransmission(1);

    *x = buffer[0] << 8 | buffer[1];
}

void getAccelerometerY(short *y)
{
    unsigned char buffer[2];

    beginTransmission(MPU6050_ADDRESS);
    write(MPU6050_RA_ACCEL_YOUT_H);
    endTransmission(0);

    requestFrom(MPU6050_ADDRESS, 2, buffer);
    endTransmission(1);

    *y = buffer[0] << 8 | buffer[1];
}

void getAccelerometer(int sensor, short *x, short *y)
{
    unsigned char buffer[4];
    unsigned char address = MPU6050_ADDRESS + sensor; // sensor 0: 0x68, sensor 1: 0x69

    beginTransmission(address);
    write(MPU6050_RA_ACCEL_XOUT_H);
    endTransmission(0);

    requestFrom(address, 4, buffer);
    endTransmission(1);

    *x = buffer[0] << 8 | buffer[1];
    *y = buffer[2] << 8 | buffer[3];
}
