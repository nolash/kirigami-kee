#ifndef _KEE_DIGEST_H
#define _KEE_DIGEST_H

#ifdef __cplusplus
extern "C" {
#endif

	int calculate_digest(const char *in, size_t in_len, char *out); //, size_t digest_len);

#ifdef __cplusplus
}
#endif

#endif // _KEE_DIGEST_H
