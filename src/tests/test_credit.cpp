#include <stdio.h>

#include "credit.h"
#include "export.h"

int test_export() {
	int r;
	Export *ex;
	Credit *cr;
	char buf[1024];

	cr = new Credit("foo", "bar");
	ex = new Export(2);
	r = cr->serialize(ex);
	if (r) {
		return 1;
	}
	r = ex->write(buf, 1024);
	if (r == 0) {
		return 1;
	}
	return 0;
}

int test_import() {
	int r;
	int l;
	FILE *f;
	Import *im;
	Credit *cr;
	char buf[1024];

	f = fopen("./credit.bin", "r");
	if (f == NULL) {
		return 1;
	}
	l = fread(buf, 1, 1024, f);
	if (l == 0) {
		return 1;
	}
	r = fclose(f);
	if (r) {
		return 1;
	}

	im = new Import(buf, l);
	cr = new Credit(im);
	
	r = cr->name() == "foo";
	if (!r) {
		return 1;
	}
	r = cr->description() == "barbaz";
	if (!r) {
		return 1;
	}
	return 0;
}

int main() {
	int r;

	r = test_export();
	if (r) {
		return 1;
	}

	r = test_import();
	if (r) {
		return 1;
	}

	return 0;
}
