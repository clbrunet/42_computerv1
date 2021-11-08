#include <stdbool.h>

bool is_number_zero(double n)
{
	if (-0.0000001 < n && n < 0.0000001) {
		return true;
	}
	return false;
}

bool is_numbers_equals(double n1, double n2)
{
	if (-0.0000001 < (n1 - n2) && (n1 - n2) < 0.0000001) {
		return true;
	}
	return false;
}

double ft_sqrt(double x)
{
	if (x < 0) {
		return -1;
	}
	double sqrt = x / 2;
	double next_x_divisor = 4;
	double x_10_exponent_7 = x * 10000000;
	while (next_x_divisor < x_10_exponent_7) {
		double pow = sqrt * sqrt;
		if (is_numbers_equals(pow, x)) {
			return sqrt;
		} else if (pow < x) {
			sqrt += x / next_x_divisor;
		} else {
			sqrt -= x / next_x_divisor;
		}
		next_x_divisor *= 2;
	}
	return sqrt;
}
