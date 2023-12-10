#include <string.h>
#include <stddef.h>
#include <qrencode.h>
#include <magick/api.h>

#include "qr.h"

int initialized = 0;


int qr_img_decode(char *in, size_t in_len, char *out, size_t out_len, size_t *width, size_t *height) {
	int i;	
	size_t l;
	Image *img;
	ImageInfo *img_info;
	ExceptionInfo img_ex;
	const PixelPacket *pp;

	if (!initialized) {
		InitializeMagick(NULL);
		initialized = 1;
	}

	img_info = CloneImageInfo(NULL);
	GetExceptionInfo(&img_ex);
	//strcpy(img_info->filename, "foo.png");
	//img = ReadImage(img_info, &img_ex);
	img = BlobToImage(img_info, in, in_len, &img_ex);
	if (img == NULL) {
		return 1;
	}
	SetImageOpacity(img, 0);
	SetImageDepth(img, 1);

	pp = AcquireImagePixels(img, 0, 0, img->columns, img->rows, &img_ex);
	if (pp == NULL) {
		return 1;
	}

	l = img->columns * img->rows;
	if (out_len < l) {
		return 2;
	}
	for (i = 0; i < (int)l; i++) {
		if ((pp+i)->blue == 65535) {
			*(out+i) = 0xff;
		} else {
			*(out+i) = 0x00;
		}
	}
	*width = img->columns;
	*height = img->rows;

	return 0;
}


int qr_encode(char *in, char *out, size_t out_len) {
	QRcode *qr;

	qr = QRcode_encodeString8bit((const char*)in, QRSPEC_VERSION_MAX, QR_ECLEVEL_M);
	if (qr == NULL) {
		return 1;
	}

	QRcode_free(qr);
	
	return 0;	
}
