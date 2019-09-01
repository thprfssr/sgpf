#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#define INTERVAL_SIZE 10000000

bool *BASIS = NULL;

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

uint64_t isqrt(uint64_t n)
{
	uint64_t r = (uint64_t) floor(sqrt((double) n));
	return r;
}

bool *get_basis(uint64_t n)
{
	uint64_t r = isqrt(n);
	bool *list = NULL;
	list = malloc((r + 1) * sizeof(bool));
	if (list == NULL) {
		printf("Error in get_basis(): Could not allocate enough memory for list! Exiting...\n");
		exit(-1);
	}

	for (int i = 0; i <= r; i++) {
		list[i] = is_prime(i);
	}

	return list;
}


uint64_t *create_sief(uint64_t n)
{
	uint64_t *sief = NULL;
	sief = malloc(n * sizeof(uint64_t));
	if (sief == NULL) {
		printf("Error in create_sief(): Could not allocate enough memory for sief! Exiting...\n");
		exit(-1);
	}

	return sief;
}

void sief_set_zero(uint64_t *sief, uint64_t size)
{
	for (uint64_t i = 0; i < size; i++) {
		sief[i] = 0;
	}
}

uint64_t sief_sum(uint64_t *sief, uint64_t size)
{
	uint64_t s = 0;
	for (uint64_t i = 0; i < size; i++)
		s += sief[i];

	return s;
}

uint64_t partial_sum_greatest_prime_factors(uint64_t a, uint64_t b, uint64_t *sief, uint64_t size)
{
	if (b <= a) {
		printf("Error in partial_sum_greatest_prime_factors(): Upper bound must be strictly greater than lower bound! Exiting...\n");
		exit(-1);
	}
	if (b - a > size) {
		printf("Error in partial_sum_greatest_prime_factors(): Sief size must be greater than or equal to b - a. Exiting...\n");
		exit(-1);
	}

	printf("Summing between %"PRIu64" and %"PRIu64"...\n", a, b);

	sief_set_zero(sief, size);
	uint64_t r = isqrt(b);
	bool *basis = BASIS;
	for (int p = 0; p <= r; p++) {
		if (basis[p] == false) {
			continue;
		}

		/*
		if (p < 1000) {
			printf("Sieving multiples of %i...\n", p);
		} else if (p == 1013) {
			printf("Marking multiples of higher primes...\n");
		}
		*/

		uint64_t i = a;
		while (i % p != 0) {
			i++;//shianne
		}
		if (i <= p) {
			i += p;
		}
		while (i < b) {
			sief[i - a] = p;
			i += p;
		}
	}

	// This procedure doesn't take into account the primes
	// nor the multiples of primes greater than r.
	// Thus, we take them into account
	for (int i = a; i < b; i++) {
		if (sief[i - a] == 0 && i > 1) {
			sief[i - a] = i;
			uint64_t j = i;
			while (j < b) {
				sief[j - a] = i;
				j += i;
			}
		}
	}

	// But we don't wanna include the 0 and 1 cases
	if (a == 1) {
		sief[1 - a] = 0;
	} else if (a == 0) {
		sief[0 - a] = 0;
		sief[1 - a] = 0;
	}

	return sief_sum(sief, size);
}

uint64_t total_sum_greatest_prime_factors(uint64_t n)
{
	BASIS = get_basis(n);
	uint64_t *sief = create_sief(INTERVAL_SIZE);

	uint64_t q = n / INTERVAL_SIZE;
	uint64_t r = n % INTERVAL_SIZE;

	uint64_t s = 0;
	for (uint64_t k = 0; k < q; k++) {
		s += partial_sum_greatest_prime_factors(k * INTERVAL_SIZE, (k + 1) * INTERVAL_SIZE, sief, INTERVAL_SIZE);
	}
	if (r > 0) {
		s += partial_sum_greatest_prime_factors(q * INTERVAL_SIZE, n, sief, INTERVAL_SIZE);
	}

	printf("The total sum of all the greatest prime factors strictly less than %" PRIu64 " is:\n%" PRIu64 "\n", n, s);
	return s;
}

int main(int *argc, char **argv)
{
	uint64_t n = strtoull(argv[1], NULL, 10);
	total_sum_greatest_prime_factors(n);


	return 0;
}
