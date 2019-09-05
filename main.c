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
#include "gpf.h"

int main(int *argc, char **argv)
{
	/* Obtain an integer as an argument. */
	//uint64_t n = strtoull(argv[1], NULL, 10);
	
	/* Obtain the bounds within which to work. */
	uint64_t a = strtoull(argv[1], NULL, 10);
	uint64_t b = strtoull(argv[2], NULL, 10);

	/* Obtain the interval size as an argument. */
	//uint64_t interval_size = strtoull(argv[2], NULL, 10);
	uint64_t interval_byte_size = strtoull(argv[3], NULL, 10);
	uint64_t interval_size = interval_byte_size / sizeof(uint64_t);

	/* Start chugging. */
	//total_sum_gpf(n, interval_size);
	total_sum_gpf(a, b, interval_size);


	return 0;
}
