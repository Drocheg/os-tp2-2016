#include <math.h>



int64_t ipow(int64_t base, uint64_t exp) {

	int64_t result = 1;
	while (exp > 0) {
		result *= base;
		exp--;
	}
	return result;
}



uint64_t decimalDigits(int64_t number) {

	uint64_t result = 0;
	if (number == 0) {
		return 1;
	}
	while (number != 0) {
		number /= 10;
		result++;
	}
	return result;
}



