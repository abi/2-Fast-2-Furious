#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bench-utils.h"

int
main (int argc, char *argv[])
{
	//int fd = 
	//read ();
	char *str = get_huge_string ();
	int len = strlen (str);
	printf ("%d\n", len);
	return 1;
}