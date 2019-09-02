#ifndef BASIS_H
#define BASIS_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include "arithmetic.h"

bool *BASIS = NULL;

/* A basis is nothing more than an array of bools, which tells us the primality
 * of each integer up to the square root of n. */
bool *get_basis(uint64_t n);

#endif
