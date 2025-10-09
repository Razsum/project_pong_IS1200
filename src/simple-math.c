#include "simple-math.h"

uint32_t rng_state = 12345;
uint32_t frame_counter = 0;

float simple_cos(float x) {
    // Taylor series: cos(x) ≈ 1 - x²/2 + x⁴/24
    float x2 = x * x;
    return 1.0f - x2 * 0.5f + x2 * x2 * 0.041666f;
}

float simple_sin(float x) {
    // Taylor series: sin(x) ≈ x - x³/6 + x⁵/120
    float x2 = x * x;
    return x * (1.0f - x2 * 0.166666f + x2 * x2 * 0.008333f);
}

void simple_srand(uint32_t seed) {
    rng_state = seed;
}

uint32_t simple_rand(void) {
    // Linear congruential generator
    rng_state = rng_state * 1103515245 + 12345;
    return (rng_state / 65536) % 32768;
}