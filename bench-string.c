#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bench-utils.h"

int
main (int argc, char *argv[])
{
	//int fd = 
	//read ();
	char *str = "Hi there"; 
	//get_huge_string ();
	int len = strlen (str);
	printf ("%d\n", len);
	
	int x = 10, y = 0;
	char *res;
	
	printf ("Y: %d\n", y);
	
	printf ("%p\n", str);
  
	//"add %1, %%ebx;"
	asm ("mov %%al, 0x0\n\t"
				"mov %%cx, 0x-1\n\t"
				"cld\n\t"
				"repne scasb\n\t"
				"mov %%ax, 0x-2\n\t"
				"sub %%ax, %%cx\n\t"
				:"=a"(res)	/* y is output operand */
				:"D"(str));	/* x is input operand */
				//:"%ebx");	/* %eax is clobbered register */
	
	printf ("%p\n", res);
	
	// asm ("cld\n
  // 	      rep\n
  // 	      movsb"
  // 	      : /* no input */
  // 	      :"S"(src), "D"(dst), "c"(count));
		
	//printf ("Y: %d\n", y);
	
	return 1;
}