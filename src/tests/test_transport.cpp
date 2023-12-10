#include <stdio.h>
#include <string.h>

#include "transport.h"
#include "qr.h"
#include "zbar.h"

int test_qr_img_file_in() {
	int r;
	char out[512 * 1024];
	size_t w;
	size_t h;
	zbar::zbar_image_t *zimg;
	zbar::zbar_image_scanner_t *zscan;
	const zbar::zbar_symbol_t *zsym;
	const char *zdata;

	r = qr_file_decode("qr.png", out, 512 * 1024, &w, &h);
	if (r) {
		return 1;
	}

	zimg = zbar::zbar_image_create();
	if (zimg == NULL) {
		return 1;
	}
	zbar::zbar_image_set_data(zimg, out, r, NULL);
	zbar::zbar_image_set_format(zimg, zbar_fourcc('Y', '8', '0', '0'));
	zbar::zbar_image_set_size(zimg, w, h);
	zscan = zbar::zbar_image_scanner_create();
	zbar_image_scanner_set_config(zscan, zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
	r = zbar_scan_image(zscan, zimg);
	if (r <= 0) {
		return 1;
	}
	
	zsym = zbar_image_first_symbol(zimg);
	zdata = zbar_symbol_get_data(zsym);
	if (strcmp(zdata, "eNpLy89PSixKSqxiAAAWMgO3")) {
		return 1;
	}

	zbar::zbar_image_destroy(zimg);

	return 0;
}
int test_qr_img_data_in() {
	int r;
	int l;
	int i;
	FILE *f;
	char in[512 * 1024];
	char out[512 * 1024];
	size_t w;
	size_t h;
	zbar::zbar_image_t *zimg;
	zbar::zbar_image_scanner_t *zscan;
	const zbar::zbar_symbol_t *zsym;
	const char *zdata;

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

	r = qr_data_decode(in, (size_t)l, out, 512 * 1024, &w, &h);
	if (r) {
		return 1;
	}

	zimg = zbar::zbar_image_create();
	if (zimg == NULL) {
		return 1;
	}
	zbar::zbar_image_set_data(zimg, out, r, NULL);
	zbar::zbar_image_set_format(zimg, zbar_fourcc('Y', '8', '0', '0'));
	zbar::zbar_image_set_size(zimg, w, h);
	zscan = zbar::zbar_image_scanner_create();
	zbar_image_scanner_set_config(zscan, zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
	r = zbar_scan_image(zscan, zimg);
	if (r <= 0) {
		return 1;
	}
	
	zsym = zbar_image_first_symbol(zimg);
	zdata = zbar_symbol_get_data(zsym);
	if (strcmp(zdata, "eNpLy89PSixKSqxiAAAWMgO3")) {
		return 1;
	}

	zbar::zbar_image_destroy(zimg);

	return 0;
}

int test_qr_out() {
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

	r = qr_encode(out, cmp, 1024);
	if (r) {
		return r;
	}

	return 0;

}

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
