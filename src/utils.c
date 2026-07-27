#include "utils.h"

bool is_alpha(int value)
{
	return (value >= 'a' && value <= 'z')
		|| (value >= 'A' && value <= 'Z');
}

bool is_num(int value)
{
	return value >= '0' && value <= '9';
}

bool is_alphanum(int value)
{
	return is_alpha(value) || is_num(value);
}
