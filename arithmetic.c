#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include "arithmetic.h"

/* A simple primality check. It should be used sparingly, lest you want the
 * program to take forever. */
bool is_prime(uint64_t n)
{
	if (n <= 1) {
		return false;
	}
	for (int i = 2; i * i <= n; i++) {
		if (n % i == 0) {
			return false;
		}
	}
	return true;
}

/* Return the integer square root. */
uint64_t isqrt(uint64_t n)
{
	uint64_t r = (uint64_t) floor(sqrt((double) n));
	return r;
}

/* Return n / f^m, where f^m is the greatest power of f which divides n. */
uint64_t divide_out(uint64_t n, uint64_t f)
{
	if (n == 0) {
		return n;
	}
	if (f == 0) {
		printf("Error in divide_out(): Division by zero! Exiting...\n");
		exit(-1);
	}
	while (n % f == 0) {
		n /= f;
	}
	return n;
}

/* Returns the smallest multiple of n not less than a. */
uint64_t smallest_multiple_not_less_than(uint64_t n, uint64_t a)
{
	uint64_t i = a;
	while (i % n != 0)
		i++;

	return i;
}

/* Returns the smallest multiple of n not less than a, excluding n itself, and
 * the special case of 0. */
uint64_t smallest_strict_multiple_not_less_than(uint64_t n, uint64_t a)
{
	uint64_t i = smallest_multiple_not_less_than(n, a);
	while (i <= n)
		i += n;

	return i;
}
