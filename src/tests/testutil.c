#include <stdlib.h>
#include "testutil.h"


int testutil_init() {
	srand(42);
	return 0;
}

int testutil_get_rand(char *out, size_t length) {
	int *p;
	int i;
	unsigned int c;
	unsigned int l;

	c = length;
	p = (int*)out;

	for (i = 0; i < length; i+=4) {
		if (c < 4) {
			l = c;
		} else {
			l = 4;
		}
		*p = rand();
		p++;
		c -= 4;
	}
	return 0;
}
