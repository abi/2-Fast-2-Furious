#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define MEM_SIZE (1 << 28)
int
main (int argc, char *argv[])
{
	int i;
	char *buf = malloc (MEM_SIZE);
	
	for (i = 0; i < MEM_SIZE; i++)
		buf[i] = (i * i) % 100;
	
	printf ("%d\n", MEM_SIZE);
	
	free (buf);
	
	return 0;
}