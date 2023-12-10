#ifndef _QR_H
#define _QR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int qr_encode(char *in, char *out, size_t out_len);
int qr_img_decode(char *in, size_t in_len, char *out, size_t out_len, size_t *w, size_t *h);

#ifdef __cplusplus
}
#endif


#endif // _QR_H
