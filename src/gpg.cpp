#include <gcrypt.h>
#include <string.h>
#include <stdio.h>

#include "err.h"
#include "debug.h"
#include "gpg.h"

#define GPG_MIN_VERSION "1.10.2"

#define BUFLEN 1024 * 1024

const char *gpgVersion = nullptr;


int key_from_data(gcry_sexp_t *key, const char *v) {
	gcry_error_t e;

	e = gcry_sexp_new(key, v, strlen(v), 1);
	if (e != GPG_ERR_NO_ERROR) {
		debugLog(DEBUG_DEBUG, v);
		return ERR_NOKEY;
	}
	return ERR_OK;
}

int key_from_path(gcry_sexp_t *key, const char *p) {
	int r;
	char v[BUFLEN];
	char *pv;
	size_t c;
	size_t i;
	FILE *f;

	f = fopen(p, "r");
	if (f == NULL) {
		return ERR_NOKEY;
	}

	i = 0;
	c = 1;
	pv = v;
	while (c != 0 && i < BUFLEN) {
		c = fread(pv+i, 1, 1024, f);
		i += c;
	}
	if (i == 0) {
		return ERR_NOKEY;
	}
	fclose(f);
	r = key_from_data(key, v);
	return r;
}

int sign(gcry_sexp_t *out, gcry_sexp_t *key, const char *v) {
	char in[BUFLEN];

	sprintf(in, "(data(flags eddsa(hash-algo sha256(value %s))))", v);
	debugLog(DEBUG_TRACE, in);

	return 0;
}

int GpgStore::check(std::string p) {
	int r;
	const char *v;
	char d[1024];
	gcry_sexp_t k;
	gcry_sexp_t o;

	if (gpgVersion == nullptr) {
		v = gcry_check_version(GPG_MIN_VERSION);
		if (v == nullptr) {
			return ERR_NOCRYPTO;
		}
	}
	gpgVersion = v;
	sprintf(d, "Using gpg version: %s", gpgVersion);
	debugLog(DEBUG_INFO, d);
	r = key_from_path(&k, p.c_str());
	if (r != ERR_OK) {
		return r;
	}
	r = sign(&o, &k, "foo");
	return r;
}
