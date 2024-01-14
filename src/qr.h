#ifndef _QR_H
#define _QR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int qr_encode(char *in, char *out, size_t *out_len);
int qr_decode_pixels(char *in, size_t in_len, char *out, size_t out_len);
int qr_decode_pixels_file(const char *filename, char *out, size_t out_len);
int qr_y800_decode(char *in, size_t in_len, size_t width, size_t height, char *out, size_t *out_len);

#ifdef __cplusplus
}
#endif

#endif // _QR_H
