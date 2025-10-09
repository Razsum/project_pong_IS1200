#ifndef SIMPLE_MATH_H
#define SIMPLE_MATH_H

#include <stdint.h>

#define M_PI 3.14159265358979323846

// Simple pseudo-random number generator
extern uint32_t rng_state;
extern uint32_t frame_counter;

// Simple cosine approximation (good enough for small angles)
extern float simple_cos(float x);

extern float simple_sin(float x);

extern void simple_srand(uint32_t seed);

extern uint32_t simple_rand(void);

#endif