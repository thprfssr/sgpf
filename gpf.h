#ifndef GPF_H
#define GPF_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <gmp.h>

#include "arithmetic.h"
#include "basis.h"

/* Check that several important assumptions hold. These assumptions are made for
 * the function partial_sum_gpf(). If one of the assumptions
 * fails, then the program simply dies. */
void partial_sum_assumption_check(uint64_t a, uint64_t b, uint64_t *slate, uint64_t slate_size);

/* Take the greatest prime factor of each integer in the interval [a, b), and
 * sum them all up. This algorithm requires a slate and its size. This is
 * basically just a slate on which to perform calculations.
 *
 * It is assumed that we always stay within the bounds of the basis.
 *
 * The sum is returned as a decimal string. */
char* partial_sum_gpf(uint64_t a, uint64_t b, uint64_t *slate, uint64_t slate_size, bool *basis);

/* Apply the partial_sum_great_prime_factors() function in adjacent intervals
 * in order to get a total sum of one grand interval. */
char* total_sum_gpf(uint64_t n, uint64_t interval_size);


#endif
