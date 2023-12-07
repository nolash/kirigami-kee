#ifndef _TRANSPORT_H
#define _TRANSPORT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int pack(char *in, size_t in_len, char *out, size_t *out_len);
int unpack(char *in, size_t in_len, char *out, size_t *out_len);

#ifdef __cplusplus
}
#endif

#endif // _TRANSPORT_H
