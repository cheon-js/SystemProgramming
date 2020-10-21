#include <stdio.h>
#include <stdlib.h>
#include "my_assert.h"

void foo(int num)
{
	my_assert(((num >=0) && (num <= 100)));
	printf("foo : num = %d\n", num);
}

int main(int argc, char *argv[])
{
	int num;

	if(argc < 2) {
		fprintf(stderr, "Usage: asstest aNumber\n(0 <= aNumber <=\n");
		exit(1);
}

	num = atoi(argv[1]);
	foo(num);
}
