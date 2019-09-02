#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <gmp.h>

#include "arithmetic.h"
#include "basis.h"
#include "slate.h"

/* DISCUSSION: This is the algorithm used to find the greatest prime factors.
 * Start by making a list of every integer in the partial interval [a, b). Since
 * 0 and 1 mess everything up, we're going to overwrite them both with a 0.
 *
 * Start with the prime p = 2. Then, for every multiple of 2 in the list,
 * including 2 itself, divide and keep dividing by 2 until you can't divide
 * anymore. There should be some 1's left, but not all of them are 1's. If you
 * end up with 1, it means that the number was a power of 2, and consequently,
 * its greatest prime factor must be 2. Hence, we should count the number of
 * ones, multiply that quantity by 2, and add it to our running sum. Then set
 * all the 1's to 0's.
 *
 * Then, do the same thing with the next prime p = 3. If you encounter a number
 * which you already divided by 2, then keep it as it is, and divide by 3 as if
 * nothing. Count all the 1's that appeared, multiply this quantity by 3, and
 * add it to the running sum. Set all the 1's to 0's.
 *
 * Repeat the same procedure for every prime less than or equal to the integer
 * square root of b. Then, what's left must either zeroes, or primes. We can now
 * safely add everything up, and add it to our running sum.
 *
 * Once you're done, the running sum should be equal to the actual sum of
 * greatest prime factors. */



/* A slate is simply an array of integers. It's basically just a blank slate on
 * which we will perform our future calculations. */
uint64_t *create_slate(uint64_t n)
{
	/* Allocate enough memory for the slate. */
	uint64_t *slate = NULL;
	slate = malloc(n * sizeof(uint64_t));
	if (slate == NULL) {
		printf("Error in create_slate(): Could not allocate enough memory for slate! Exiting...\n");
		exit(-1);
	}

	return slate;
}

/* Set each slate entry to zero. */
void slate_set_zero(uint64_t *slate, uint64_t slate_size)
{
	for (uint64_t i = 0; i < slate_size; i++) {
		slate[i] = 0;
	}
}

/* Sum up all the slate entries, and return the total sum as a decimal string */
char* slate_sum(uint64_t *slate, uint64_t slate_size)
{
	/* Declare a GMP integer, and initialize it to 0. */
	mpz_t s;
	mpz_init(s);

	/* Add up all the slate entries. */
	for (uint64_t i = 0; i < slate_size; i++) {
		mpz_add_ui(s, s, slate[i]);
	}

	/* Convert s to a decimal string. Exit if the string is NULL. Finally,
	 * clear the variable s. */
	char *str = NULL;
	str = mpz_get_str(str, 10, s);
	mpz_clear(s);
	if (str == NULL) {
		printf("Error in slate_sum(): str is NULL! Exiting...\n");
		exit(-1);
	}

	/* Return the total sum as a decimal string. */
	return str;
}

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
void slate_set_consecutive(uint64_t *slate, uint64_t slate_size, uint64_t a, uint64_t b)
{
	for (uint64_t i = a; i < b; i++)
	{
		slate[i - a] = i;
	}

	/* Set the remaining elements to zero. It is more efficient to zero
	 * these few elements at the end rather than to zero every element in
	 * the beginning. */
	for (uint64_t i = b; i - a < slate_size; i++) {
		slate[i - a] = 0;
	}
}
