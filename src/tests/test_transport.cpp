#include <stdio.h>
#include <string.h>

#include "transport.h"
#include "qr.h"
#include "zbar.h"


/// test read of qr file
int test_qr_img_file_in() {
	int r;
	char out[512 * 1024];

	r = qr_decode_pixels_file("qr.png", out, 512 * 1024);
	if (r) {
		return 1;
	}
	if (strcmp(out, "eNpLy89PSixKSqxiAAAWMgO3")) {
		return 1;
	}

	return 0;
}


/// test read of qr data from memory
int test_qr_img_data_in() {
	int r;
	int l;
	int i;
	FILE *f;
	char in[512 * 1024];
	char out[512 * 1024];

	f = fopen("qr.png", "rb");
	if (f == NULL) {
		return 1;
	}
	i = 0;
	while (1) {
		r = fread(in+i, 1, 1024, f);
		l += r;
		if (r < 1024) {
			break;
		}
		i += l;
	}
	fclose(f);

	r = qr_decode_pixels(in, (size_t)l, out, 512 * 1024);
	if (r) {
		return 1;
	}
	if (strcmp(out, "eNpLy89PSixKSqxiAAAWMgO3")) {
		return 1;
	}

	return 0;
}


/**
 * test generate qr code
 * \todo check output matches input
 */
int test_qr_out() {
	int r;
	size_t l;
	size_t c;
	char in[] = "foobarbaz";

	l = 128*1024;
	c = 1024;
	char out[128*1024];
	char cmp[1024];

	r = pack(in, strlen(in) + 1, out, &c);
	if (r) {
		return 1;
	}

	r = qr_encode(out, cmp, &l);
	if (r) {
		return r;
	}

	return 0;

}


/// check that encode and decode for transport is symmetric
int test_roundtrip() {
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

int main() {
	int r;
	r = test_roundtrip();
	if (r) {
		return r;
	}

	r = test_qr_out();
	if (r) {
		return r;
	}

	r = test_qr_img_file_in();
	if (r) {
		return r;
	}

	r = test_qr_img_data_in();
	if (r) {
		return r;
	}

	return 0;
}
