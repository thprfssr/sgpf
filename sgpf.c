#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#define INTERVAL_SIZE 10000000

/* FIXME: The final result that the program spits out is dependent on the
 * chosen interval size. */

bool *BASIS = NULL;

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

/* A sief is simply an array of integers. It's basically just a blank slate on
 * which we will perform our future calculations. */
uint64_t *create_sief(uint64_t n)
{
	/* Allocate enough memory for the sief. */
	uint64_t *sief = NULL;
	sief = malloc(n * sizeof(uint64_t));
	if (sief == NULL) {
		printf("Error in create_sief(): Could not allocate enough memory for sief! Exiting...\n");
		exit(-1);
	}

	return sief;
}

/* Set each sief entry to zero. */
void sief_set_zero(uint64_t *sief, uint64_t sief_size)
{
	for (uint64_t i = 0; i < sief_size; i++) {
		sief[i] = 0;
	}
}

/* Sum up all the sief entries. */
uint64_t sief_sum(uint64_t *sief, uint64_t sief_size)
{
	uint64_t s = 0;
	for (uint64_t i = 0; i < sief_size; i++)
		s += sief[i];

	return s;
}

/* Take the greatest prime factor of each integer in the interval [a, b), and
 * sum them all up. This algorithm requires a sief and its size. This is
 * basically just a slate on which to perform calculations. */
uint64_t partial_sum_greatest_prime_factors(uint64_t a, uint64_t b, uint64_t *sief, uint64_t sief_size)
{
	/* Check that several assumptions hold. If they don't, then the program
	 * simply dies. */
	if (b <= a) {
		printf("Error in partial_sum_greatest_prime_factors(): Upper bound must be strictly greater than lower bound! Exiting...\n");
		exit(-1);
	}
	if (b - a > sief_size) {
		printf("Error in partial_sum_greatest_prime_factors(): Sief size must be greater than or equal to b - a. Exiting...\n");
		exit(-1);
	}

	/* Let the user know that everything is running smoothly. */
	printf("Summing between %"PRIu64" and %"PRIu64"...\n", a, b);

	/* In order to obtain a list of the greatest prime factor of each
	 * integer within the interval [a, b), we first set every sief entry to
	 * zero. This will basically be a blank slate on which we will perform
	 * our calculations.
	 *
	 * Then, for each multiple of 2, we write a 2 in the corresponding sief
	 * entry. This means that the greatest prime factor so far is 2.
	 *
	 * After that, we move on to 3, and we carry out the same process,
	 * overwriting past entries if necessary. This overwriting is good,
	 * because it means that we have found a greater prime factor than the
	 * previous one.
	 *
	 * Then, keep going, until all the primes less than the square root of
	 * b have been used. */
	sief_set_zero(sief, sief_size);
	uint64_t r = isqrt(b);
	bool *basis = BASIS;
	for (int p = 0; p <= r; p++) {
		if (basis[p] == false) {
			continue;
		}

		/* Let i be the smallest multiple of p, which must not be p
		 * itself, and which must be within the interval [a, b). It
		 * must not b itself, because we're dealing with primes in a
		 * later part of this function. */
		uint64_t i = a;
		while (i % p != 0) {
			i++;
		}
		while (i <= p) {
			i += p;
		}

		/* For each multiple of p, we write p itself into the
		 * corresponding sief entry. */
		while (i < b) {
			sief[i - a] = p;
			i += p;
		}
	}

	/* However, the procedure outlined above does not take into account
	 * the primes which are greater than the square root of b. Hence, we
	 * must now take care of them. And since I was gonna take care of
	 * primes here, I might as well take care of the primes which are less
	 * than or equal to the square root of b. */
	for (int i = a; i < b; i++) {
		if (sief[i - a] == 0 && i - a > 1) {
			uint64_t j = i;
			while (j < b) {
				sief[j - a] = i;
				j += i;
			}
		}
	}

	/* Just in case that the procedure above messed up with the special
	 * cases of 0 and 1, we take care of them here. For our purposes, zero
	 * is the greatest prime factor of both 0 and 1. */
	if (a == 1) {
		sief[1 - a] = 0;
	} else if (a == 0) {
		sief[0 - a] = 0;
		sief[1 - a] = 0;
	}

	/* Finally, return the sum of the sief entries. */
	return sief_sum(sief, sief_size);
}

/* Apply the partial_sum_great_prime_factors() function in adjacent intervals
 * in order to get a total sum of one grand interval. */
uint64_t total_sum_greatest_prime_factors(uint64_t n)
{
	/* Create the basis and sief to be used. */
	BASIS = get_basis(n);
	uint64_t *sief = create_sief(INTERVAL_SIZE);

	/* Divide up the interval [0, n) into smaller intervals, given by the
	 * global INTERVAL_SIZE. */
	uint64_t q = n / INTERVAL_SIZE;
	uint64_t r = n % INTERVAL_SIZE;

	/* Start adding within each interval, and after that, take care of the
	 * last interval. */
	uint64_t s = 0;
	for (uint64_t k = 0; k < q; k++) {
		s += partial_sum_greatest_prime_factors(k * INTERVAL_SIZE, (k + 1) * INTERVAL_SIZE, sief, INTERVAL_SIZE);
	}
	if (r > 0) {
		s += partial_sum_greatest_prime_factors(q * INTERVAL_SIZE, n, sief, INTERVAL_SIZE);
	}

	/* Finally, let the user know the grand sum, and return it. */
	printf("The total sum of all the greatest prime factors strictly less than %" PRIu64 " is:\n%" PRIu64 "\n", n, s);
	return s;
}

int main(int *argc, char **argv)
{
	/* Obtain an integer as an argument. */
	uint64_t n = strtoull(argv[1], NULL, 10);

	/* Start chugging. */
	total_sum_greatest_prime_factors(n);


	return 0;
}
