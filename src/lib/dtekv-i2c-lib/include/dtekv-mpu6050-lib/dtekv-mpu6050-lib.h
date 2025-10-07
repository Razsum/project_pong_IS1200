#ifndef _DTEKV_MPU6050_LIB_H_
#define _DTEKV_MPU6050_LIB_H_

#define MPU6050_ADDRESS 0b1101000
#define NOPS_PER_MS 10000

void initializeSensor(int sensor);

void getAccelerometerX(short *x);
void getAccelerometerY(short *y);
void getAccelerometer(int sensor, short *x, short *y);

#endif