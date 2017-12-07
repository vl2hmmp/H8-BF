#include "utils.h"

int clamp(int value, int min, int max)
{
	value = max < value ? max : value;
	value = value < min ? min : value;
	return value;
}
