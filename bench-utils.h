#include <string.h>

#define HUGE_STRING_SIZE (1 << 26)

char*
get_huge_string (void)
{
	char *huge_string = malloc (HUGE_STRING_SIZE);
	memset (huge_string, '3', HUGE_STRING_SIZE);
	huge_string[HUGE_STRING_SIZE - 2] = '\0';
	return huge_string;
}