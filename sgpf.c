#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <gmp.h>

#define INTERVAL_SIZE 10000000

/* FIXME: The final result that the program spits out is dependent on the
 * chosen interval size. */

/* DISCUSSION: AH! I think I found the cause. Let N = 40, and let the interval
 * size be 20. The first partial interval of [0, 20) goes smoothly, but let's
 * take a look at the second partial interval of [20, 40). The integer square
 * root of 40 is 6, so we only need to consider the primes {2, 3, 5} according
 * to my algorithm. We start writing `2` into the entry of each multiple of 2,
 * and likewise we write `3`, and `5`. Then the empty entries that remain must
 * be prime numbers, so we mark them as well, along with their multiples.
 *
 * But look! According to this algorithm, the greatest prime factor of 21 is
 * supposedly 3. How alarming! My gut says that THIS is the reason that the
 * total sum is dependent on the interval size.
 *
 * This problem only arises if we're dealing with an interval that doesn't
 * start at zero. Hence, if the interval size is as big as N or bigger, then
 * this shouldn't be a problem. */

/* SOLUTION: As we're summing up each partial interval, we need to have kept
 * track of all the primes we encountered in the previous partial intervals. We
 * can keep these primes in an array. The amount of memory required for this
 * array can be determined by some magic formula that might exist, or we can
 * simply keep a file which contains pi(k) for a couple of values of k, where
 * pi is the prime counting function. We could also reallocate the array if it
 * goes overboard, but we shouldn't do that too often, because it would slow
 * the program down. */

/* PROBLEM: Storing primes is a non-trivial task. The amount of primes which
 * are below 1T is roughly 38G. And 38G * sizeof(uint64_t) = fuckton. We need a
 * compact way to store these primes. */

/* DISCUSSION: What if we used a different algorithm for finding the greatest
 * prime factors? Start by making a list of every integer in the partial
 * interval [a, b). Since 0 and 1 mess everything up, we're going to overwrite
 * them both with a 0.
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
 * greatest prime factors.
 *
 * I tried this out with a couple of random intervals which don't start at zero,
 * and it worked! I think this really might be the algorithm I need. */

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
	//uint64_t s = 0;

	/* Declare a GMP integer, and initialize it to 0. */
	mpz_t s;
	mpz_init(s);

	/* Add up all the slate entries. */
	for (uint64_t i = 0; i < slate_size; i++) {
		//s += slate[i];
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

/* Take the greatest prime factor of each integer in the interval [a, b), and
 * sum them all up. This algorithm requires a slate and its size. This is
 * basically just a slate on which to perform calculations.
 *
 * The sum is returned as a decimal string. */
char* partial_sum_greatest_prime_factors(uint64_t a, uint64_t b, uint64_t *slate, uint64_t slate_size)
{
	/* Check that several assumptions hold. If they don't, then the program
	 * simply dies. */
	if (b <= a) {
		printf("Error in partial_sum_greatest_prime_factors(): Upper bound must be strictly greater than lower bound! Exiting...\n");
		exit(-1);
	}
	if (b - a > slate_size) {
		printf("Error in partial_sum_greatest_prime_factors(): Slate size must be greater than or equal to b - a. Exiting...\n");
		exit(-1);
	}

	/* Let the user know that everything is running smoothly. */
	printf("Summing between %"PRIu64" and %"PRIu64"...\n", a, b);

	/* In order to obtain a list of the greatest prime factor of each
	 * integer within the interval [a, b), we first set every slate entry to
	 * zero. This will basically be a blank slate on which we will perform
	 * our calculations.
	 *
	 * Then, for each multiple of 2, we write a 2 in the corresponding slate
	 * entry. This means that the greatest prime factor so far is 2.
	 *
	 * After that, we move on to 3, and we carry out the same process,
	 * overwriting past entries if necessary. This overwriting is good,
	 * because it means that we have found a greater prime factor than the
	 * previous one.
	 *
	 * Then, keep going, until all the primes less than the square root of
	 * b have been used. */
	slate_set_zero(slate, slate_size);
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
		 * corresponding slate entry. */
		while (i < b) {
			slate[i - a] = p;
			i += p;
		}
	}

	/* However, the procedure outlined above does not take into account
	 * the primes which are greater than the square root of b. Hence, we
	 * must now take care of them. And since I was gonna take care of
	 * primes here, I might as well take care of the primes which are less
	 * than or equal to the square root of b. */
	for (int i = a; i < b; i++) {
		if (slate[i - a] == 0 && i - a > 1) {
			uint64_t j = i;
			while (j < b) {
				slate[j - a] = i;
				j += i;
				/* NOTE: If a is zero, then you're re-marking
				 * all the multiples of the primes which are
				 * less than the square root. This is slightly
				 * inefficient. */
			}
		}
	}

	/* Just in case that the procedure above messed up with the special
	 * cases of 0 and 1, we take care of them here. For our purposes, zero
	 * is the greatest prime factor of both 0 and 1. */
	if (a == 1) {
		slate[1 - a] = 0;
	} else if (a == 0) {
		slate[0 - a] = 0;
		slate[1 - a] = 0;
	}

	/* Finally, return the sum of the slate entries as a decimal string. */
	return slate_sum(slate, slate_size);
}

/* Apply the partial_sum_great_prime_factors() function in adjacent intervals
 * in order to get a total sum of one grand interval. */
char* total_sum_greatest_prime_factors(uint64_t n)
{
	/* Create the basis and slate to be used. */
	BASIS = get_basis(n);
	uint64_t *slate = create_slate(INTERVAL_SIZE);

	/* Divide up the interval [0, n) into smaller intervals, given by the
	 * global INTERVAL_SIZE. */
	uint64_t q = n / INTERVAL_SIZE;
	uint64_t r = n % INTERVAL_SIZE;

	/* Start adding within each interval, and after that, take care of the
	 * last interval. Use a GMP integer to handle the total sum.*/
	//uint64_t s = 0;
	mpz_t s;
	mpz_init(s);
	mpz_t tmp;
	mpz_init(tmp);
	for (uint64_t k = 0; k < q; k++) {
		char *str = partial_sum_greatest_prime_factors(k * INTERVAL_SIZE, (k + 1) * INTERVAL_SIZE, slate, INTERVAL_SIZE);
		mpz_set_str(tmp, str, 10);
		mpz_add(s, s, tmp);
		//s += partial_sum_greatest_prime_factors(k * INTERVAL_SIZE, (k + 1) * INTERVAL_SIZE, slate, INTERVAL_SIZE);
	}
	if (r > 0) {
		char *str = partial_sum_greatest_prime_factors(q * INTERVAL_SIZE, n, slate, INTERVAL_SIZE);
		mpz_set_str(tmp, str, 10);
		mpz_add(s, s, tmp);
		//s += partial_sum_greatest_prime_factors(q * INTERVAL_SIZE, n, slate, INTERVAL_SIZE);
	}

	/* Convert the sum to a decimal string, and clear all the GMP
	 * variables. Exit if the string is NULL. */
	char *str = NULL;
	str = mpz_get_str(str, 10, s);
	mpz_clear(s);
	mpz_clear(tmp);
	if (str == NULL) {
		printf("Error in total_sum_greatest_prime_factors(): str is NULL! Exiting...\n");
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

	/* Start chugging. */
	total_sum_greatest_prime_factors(n);


	return 0;
}
