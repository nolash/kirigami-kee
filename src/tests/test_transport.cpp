#include <string.h>

#include "transport.h"

int main() {
	int r;
	size_t c;
	char in[] = "foobarbaz";

	c = 1024;
	char out[1024];
	char cmp[1024];

	r = pack(in, strlen(in) + 1, out, &c);
	if (r) {
		return 1;
	}

	c = 1024;
	r = unpack(out, strlen(out), cmp, &c);
	if (r) {
		return 1;
	}
	if (c != 10) {
		return 1;
	}
	if (memcmp(cmp, in, c)) {
		return 1;
	}

	return 0;
}
