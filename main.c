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
	uint64_t n = strtoull(argv[1], NULL, 10);

	/* Obtain the interval size as an argument. */
	uint64_t interval_size = strtoull(argv[2], NULL, 10);

	/* Start chugging. */
	total_sum_gpf(n, interval_size);


	return 0;
}
