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

