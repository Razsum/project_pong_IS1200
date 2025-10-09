#ifndef SIMPLE_MATH_H
#define SIMPLE_MATH_H

#include <stdint.h>

#define M_PI 3.14159265358979323846

/**
 * Number that will be used
 */
extern uint32_t rng_state;

/**
 * Frame counter defined that is later used to mimic randomness
 */
extern uint32_t frame_counter;

/**
 * @brief Cosine approximation
 *
 * @param x Angle in radians
 * @return Approximate cosine of x
 * @author Kai Nguyen
 */
extern float simple_cos(float x);

/**
 * @brief Sinus approximation
 *
 * @param x Angle in radians
 * @return Approximate sinus of x
 * @author Kai Nguyen
 */

extern float simple_sin(float x);

/**
 * @brief Initializes the pseudo-random number generator with a seed value.
 *
 * @param seed Seed value (frame_counter * a prime number)
 * @author Kai Nguyen
 */
extern void simple_srand(uint32_t seed);

/**
 * @brief Generates the next pseudo-random number using a linear congruential generator (LCG).
 *
 * @return The next pseudo-random 32-bit value.
 * @author Kai Nguyen
 */
extern uint32_t simple_rand(void);

#endif