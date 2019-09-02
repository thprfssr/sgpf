#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include "arithmetic.h"

/* A basis is nothing more than an array of bools, which tells us the primality
 * of each integer up to the square root of n. */
bool *get_basis(uint64_t n)
{
	uint64_t r = isqrt(n);

	/* Allocate enough memory for the list of bools. */
	bool *list = NULL;
	list = malloc((r + 1) * sizeof(bool));
	if (list == NULL) {
		printf("Error in get_basis(): Could not allocate enough memory for list! Exiting...\n");
		exit(-1);
	}

	/* Set the values of the list of bools to reflect the primality of each
	 * integer within the intended range of the list. */
	for (int i = 0; i <= r; i++) {
		list[i] = is_prime(i);
	}

	return list;
}
