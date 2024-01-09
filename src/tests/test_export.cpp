#include <string.h>

#include "export.h"


/// unit-test export-import roundtrip with generic data
int main() {
	int r;
	int l;
	const char *s[] = {"foo", "barbaz"};
	const char chk[] = { 0x03, 'f', 'o', 'o', 0x06, 'b', 'a', 'r', 'b', 'a', 'z' };
	char b[1024];
	char o[1024];
	Export *ex = new Export(2);
	Import *im;

	r = 0;
	r += ex->addItem((char*)s[0], strlen(s[0]));
	r += ex->addItem((char*)s[1], strlen(s[1]));
	if (!r) {
		return 1;
	}

	r = ex->write(b, 1024);
	if (!r) {
		return 1;
	}
	l = r;
	r = memcmp(chk, b, sizeof(chk));
	if (r) {
		return 1;
	}

	im = new Import(b, l);
	r = im->read(o, 1024);
	if (!r) {
		return 1;
	}
	r = strcmp(o, "foo");
	if (r) {
		return 1;
	}
	r = im->read(o, 1024);
	if (!r) {
		return 1;
	}
	r = strcmp(o, "barbaz");
	if (r) {
		return 1;
	}

	r = im->read(o, 1024);
	if (r) {
		return 1;
	}
	return 0;	
}
