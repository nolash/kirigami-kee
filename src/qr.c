#include <string.h>
#include <stddef.h>
#include <qrencode.h>
#include <magick/api.h>

#include "qr.h"
#include "zbar.h"

#define QR_MODULE_SIZE 10
#define QR_VERSION 20

int initialized = 0;


static ImageInfo *qr_img_init(ExceptionInfo *img_ex) {
	if (!initialized) {
		InitializeMagick(NULL);
		initialized = 1;
	}
	GetExceptionInfo(img_ex);
	return CloneImageInfo(NULL);
}

static int qr_y800_decode(char *in, size_t in_len, size_t width, size_t height, char *out, size_t *out_len) {
	int r;
	zbar_image_t *zimg;
	zbar_image_scanner_t *zscan;
	const zbar_symbol_t *zsym;
	const char *zdata;

	zimg = zbar_image_create();
	if (zimg == NULL) {
		return 1;
	}
	zbar_image_set_data(zimg, in, in_len, NULL);
	zbar_image_set_format(zimg, zbar_fourcc('Y', '8', '0', '0'));
	zbar_image_set_size(zimg, width, height);
	zscan = zbar_image_scanner_create();
	zbar_image_scanner_set_config(zscan, ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	r = zbar_scan_image(zscan, zimg);
	if (r <= 0) {
		return 1;
	}
	
	zsym = zbar_image_first_symbol(zimg);
	zdata = zbar_symbol_get_data(zsym);
	strcpy(out, zdata);
	*out_len = strlen(zdata) + 1;

	zbar_image_destroy(zimg);

	return 0;
}

static int qr_img_decode(Image *img, char *out, size_t out_len) {
	int r;
	int i;	
	size_t l;
	ExceptionInfo img_ex;
	const PixelPacket *pp;
	char *px;

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
	px = malloc(l);
	for (i = 0; i < (int)l; i++) {
		if ((pp+i)->blue == 65535) {
			*(px+i) = 0xff;
		} else {
			*(px+i) = 0x00;
		}
	}

	r = qr_y800_decode(px, l, img->columns, img->rows, out, &out_len);
	free(px);
	return r;
}

int qr_decode_pixels_file(const char *filename, char *out, size_t out_len) {
	Image *img;
	ImageInfo *img_info;
	ExceptionInfo img_ex;

	img_info = qr_img_init(&img_ex);
	strcpy(img_info->filename, filename);
	img = ReadImage(img_info, &img_ex);
	if (img == NULL) {
		return 1;
	}
	return qr_img_decode(img, out, out_len);
}


int qr_decode_pixels(char *in, size_t in_len, char *out, size_t out_len) {
	Image *img;
	ImageInfo *img_info;
	ExceptionInfo img_ex;

	img_info = qr_img_init(&img_ex);
	img = BlobToImage(img_info, in, in_len, &img_ex);
	if (img == NULL) {
		return 1;
	}

	return qr_img_decode(img, out, out_len);
}


int qr_render(char *in, size_t in_width, char *out, size_t *out_len) {
	int r;
	unsigned int col;
	unsigned int row;
	unsigned int colp;
	unsigned int rowp;
	int i;
	size_t l;
	size_t w;
	ImageInfo *img_info;
	DrawInfo info;
	Image *img;
	ExceptionInfo img_ex;
	PixelPacket color;
	DrawContext ctx;
	void *buf;

	w = in_width * QR_MODULE_SIZE;
	l = in_width * in_width;

	img_info = qr_img_init(&img_ex);

//	color.red = 0xffff;
//	color.green = 0xffff;
//	color.blue = 0xffff;
//	color.opacity = 0;
	memset(&color, 0, 8);

	strcpy(img_info->magick, "png");
	img_info->colorspace = RGBColorspace;
	img_info->depth = 16;
	img_info->antialias = 0;
	img = AllocateImage(img_info);
	img->columns = w;
	img->rows = w;
	SetImage(img, 0); // assuming white bg color by default
	SetImageType(img, TrueColorType);
	SyncImage(img);

	GetDrawInfo(img_info, &info);
	ctx = DrawAllocateContext(&info, img);
	DrawSetStrokeColor(ctx, &color);
	DrawSetFillColor(ctx, &color);
	DrawSetStrokeWidth(ctx, 1);
	col = 0;
	row = 0;
	SetImageChannelDepth(img, AllChannels, 8);
	for (i = 0; i < (int)l; i++) {
		if (*(in+i) & 1) {
			colp = col * QR_MODULE_SIZE;
			rowp = row * QR_MODULE_SIZE;
			DrawRectangle(ctx, colp, rowp, colp + QR_MODULE_SIZE, rowp + QR_MODULE_SIZE);
		}
		col++;
		if (col == in_width) {
			col = 0;
			row++;	
		}
	}
	r = DrawRender(ctx);
	if (!r) {
		return 1; // returns True
	}
	SyncImage(img);

	buf = ImageToBlob(img_info, img, out_len, &img_ex);
	memcpy(out, buf, *out_len);
	
	return 0;
}

int qr_encode(char *in, char *out, size_t *out_len) {
	QRcode *qr;

	//qr = QRcode_encodeString8bit((const char*)in, QRSPEC_VERSION_MAX, QR_ECLEVEL_M);
	qr = QRcode_encodeString8bit((const char*)in, QR_VERSION, QR_ECLEVEL_M);
	if (qr == NULL) {
		return 1;
	}
	qr_render((char*)qr->data, qr->width, out, out_len);

	QRcode_free(qr);
	
	return 0;	
}
