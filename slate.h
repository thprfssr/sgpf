#ifndef SLATE_H
#define SLATE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <gmp.h>

#include "arithmetic.h"
#include "basis.h"

#define MAX_INTERVAL_SIZE 10000000

/* A slate is simply an array of integers. It's basically just a blank slate on
 * which we will perform our future calculations. */
uint64_t *create_slate(uint64_t n);

/* Set each slate entry to zero. */
void slate_set_zero(uint64_t *slate, uint64_t slate_size);

/* Sum up all the slate entries, and return the total sum as a decimal string */
char* slate_sum(uint64_t *slate, uint64_t slate_size);

/* This function sets the slate elements equal to consecutive integers between
 * a and b. That is, the slate is set to [a, a+1, a+2, ... , b-2, b-1]. Notice
 * that it stops at b-1, and doesn't include the endpoint b.
 *
 * If the range is smaller than the size, then the remaining elements will be
 * set to zero.
 *
 * This function assumes that a < b, and that b - a > slate size, and that
 * slate != NULL. The function makes no effort of actually verifying these
 * assumptions, so be careful. */
void slate_set_consecutive(uint64_t *slate, uint64_t slate_size, uint64_t a, uint64_t b);

#endif
