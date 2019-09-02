#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

/* A simple primality check. It should be used sparingly, lest you want the
 * program to take forever. */
bool is_prime(uint64_t n);

/* Return the integer square root. */
uint64_t isqrt(uint64_t n);

/* Return n / f^m, where f^m is the greatest power of f which divides n. */
uint64_t divide_out(uint64_t n, uint64_t f);

/* Returns the smallest multiple of n not less than a. */
uint64_t smallest_multiple_not_less_than(uint64_t n, uint64_t a);

/* Returns the smallest multiple of n not less than a, excluding n itself, and
 * the special case of 0. */
uint64_t smallest_strict_multiple_not_less_than(uint64_t n, uint64_t a);

#endif
