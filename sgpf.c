#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <gmp.h>

#define MAX_INTERVAL_SIZE 10000000

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

/* Check that several important assumptions hold. These assumptions are made for
 * the function partial_sum_gpf(). If one of the assumptions
 * fails, then the program simply dies. */
void partial_sum_assumption_check(uint64_t a, uint64_t b, uint64_t *slate, uint64_t slate_size)
{
	if (b <= a) {
		printf("Error in partial_sum_gpf(): Upper bound must be strictly greater than lower bound! Exiting...\n");
		exit(-1);
	}
	if (b - a > slate_size) {
		printf("Error in partial_sum_gpf(): Slate size must be greater than or equal to b - a. Exiting...\n");
		exit(-1);
	}
	if (slate == NULL) {
		printf("Error in partial_sum_gpf(): Given slate is NULL! Exiting...\n");
	}
}

/* Take the greatest prime factor of each integer in the interval [a, b), and
 * sum them all up. This algorithm requires a slate and its size. This is
 * basically just a slate on which to perform calculations.
 *
 * The sum is returned as a decimal string. */
char* partial_sum_gpf(uint64_t a, uint64_t b, uint64_t *slate, uint64_t slate_size)
{
	/* Check that several assumptions hold, or die. */
	partial_sum_assumption_check(a, b, slate, slate_size);

	/* Let the user know that everything is running smoothly. */
	printf("Summing between %"PRIu64" and %"PRIu64"...\n", a, b);

	/* Set the elements of the slate to be [a, a+1, ... , b-1]. */
	slate_set_consecutive(slate, slate_size, a, b);

	/* Take care of the special cases of 0 and 1. For our purposes with
	 * Project Euler 642, they should be set to zero. */
	if (a == 1) {
		slate[1 - a] = 0;
	} else if (a == 0) {
		slate[0 - a] = 0;
		slate[1 - a] = 0;
	}

	/* Loop through each prime in the basis. */
	uint64_t r = isqrt(b);
	bool *basis = BASIS;
	for (int p = 0; p <= r; p++) {
		if (basis[p] == false) {
			continue;
		}

		uint64_t i = smallest_multiple_not_less_than(p, a);
		while (i < b) {
			/* Divide out all the multiples of p. */
			uint64_t u = slate[i - a];
			u = divide_out(u, p);

			/* If the result is 1, then set the slate entry to p.
			 * Otherwise, set it equal to u. */
			if (u == 1)
				slate[i - a] = p;
			else
				slate[i - a] = u;

			i += p;
		}
	}
	return slate_sum(slate, slate_size);
}


/* Apply the partial_sum_great_prime_factors() function in adjacent intervals
 * in order to get a total sum of one grand interval. */
char* total_sum_gpf(uint64_t n, uint64_t interval_size)
{
	/* Create the basis and slate to be used. */
	BASIS = get_basis(n);
	if (n <= interval_size)
		interval_size = n;
	if (MAX_INTERVAL_SIZE <= interval_size)
		interval_size = MAX_INTERVAL_SIZE;
	uint64_t *slate = create_slate(interval_size);

	/* Divide up the interval [0, n) into smaller intervals, whose sizes are
	 * given by the variable `interval_size`. */
	uint64_t q = n / interval_size;
	uint64_t r = n % interval_size;

	/* Start adding within each interval, and after that, take care of the
	 * last interval. Use a GMP integer to handle the total sum.*/
	mpz_t s;
	mpz_init(s);
	mpz_t tmp;
	mpz_init(tmp);
	for (uint64_t k = 0; k < q; k++) {
		char *str = partial_sum_gpf(k * interval_size, (k + 1) * interval_size, slate, interval_size);
		mpz_set_str(tmp, str, 10);
		mpz_add(s, s, tmp);
	}
	if (r > 0) {
		char *str = partial_sum_gpf(q * interval_size, n, slate, interval_size);
		mpz_set_str(tmp, str, 10);
		mpz_add(s, s, tmp);
	}

	/* Convert the sum to a decimal string, and clear all the GMP
	 * variables. Exit if the string is NULL. */
	char *str = NULL;
	str = mpz_get_str(str, 10, s);
	mpz_clear(s);
	mpz_clear(tmp);
	if (str == NULL) {
		printf("Error in total_sum_gpf(): str is NULL! Exiting...\n");
		exit(-1);
	}

	/* Finally, let the user know the grand sum, and return it. */
	printf("The total sum of all the greatest prime factors strictly less than %" PRIu64 " is:\n%s\n", n, str);
	return str;
}

int main(int *argc, char **argv)
{
	/* Obtain an integer as an argument. */
	uint64_t n = strtoull(argv[1], NULL, 10);

	/* Obtain the interval size as an argument. */
	uint64_t interval_size = strtoull(argv[2], NULL, 10);

	/* Start chugging. */
	total_sum_gpf(n, interval_size);


	return 0;
}
