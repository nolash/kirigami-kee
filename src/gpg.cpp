#include <gcrypt.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include "err.h"
#include "debug.h"
#include "gpg.h"
#include "hex.h"

#define GPG_MIN_VERSION "1.10.2"
#define CHACHA20_KEY_LENGTH_BYTES 32
#define CHACHA20_NONCE_LENGTH_BYTES 12

#define BUFLEN 1024 * 1024

const char *gpgVersion = nullptr;


int encrypt(char *ciphertext, size_t ciphertext_len, std::string keydata, const char *key, const char *nonce) {
	gcry_cipher_hd_t h;
	gcry_error_t e;
	const char *keydata_raw;

	e = gcry_cipher_open(&h, GCRY_CIPHER_CHACHA20, GCRY_CIPHER_MODE_POLY1305, GCRY_CIPHER_SECURE);
	if (e) {
		return ERR_NOCRYPTO;
	}
	e = gcry_cipher_setkey(h, key, CHACHA20_KEY_LENGTH_BYTES);
	if (e) {
		return ERR_NOCRYPTO;
	}
	e = gcry_cipher_setiv(h, nonce, CHACHA20_NONCE_LENGTH_BYTES);
	if (e) {
		return ERR_NOCRYPTO;
	}
	keydata_raw = keydata.c_str();
	e = gcry_cipher_encrypt(h, (unsigned char*)ciphertext, ciphertext_len, (const unsigned char*)keydata_raw, keydata.length());
	if (e) {
		return ERR_NOCRYPTO;
	}
	gcry_cipher_close(h);

	return ERR_OK;
}

int decrypt(std::string *keydata, const char *ciphertext, size_t ciphertext_len, const char *key, const char *nonce) {
	gcry_cipher_hd_t h;
	gcry_error_t e;
	char keydata_raw[1024];

	e = gcry_cipher_open(&h, GCRY_CIPHER_CHACHA20, GCRY_CIPHER_MODE_POLY1305, GCRY_CIPHER_SECURE);
	if (e) {
		return ERR_NOCRYPTO;
	}
	e = gcry_cipher_setkey(h, key, CHACHA20_KEY_LENGTH_BYTES);
	if (e) {
		return ERR_NOCRYPTO;
	}
	e = gcry_cipher_setiv(h, nonce, CHACHA20_NONCE_LENGTH_BYTES);
	if (e) {
		return ERR_NOCRYPTO;
	}
	e = gcry_cipher_decrypt(h, keydata_raw, 1024, ciphertext, ciphertext_len);
	if (e) {
		return ERR_NOCRYPTO;
	}
	keydata->assign(keydata_raw);

	gcry_cipher_close(h);

	return ERR_OK;
}

int key_from_data(gcry_sexp_t *key, const char *keydata) {
	gcry_error_t e;

	e = gcry_sexp_new(key, keydata, strlen(keydata), 1);
	if (e != GPG_ERR_NO_ERROR) {
		debugLog(DEBUG_DEBUG, keydata);
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
	char *fullpath;

	fullpath = (char*)malloc(strlen(p) + 8);
	sprintf(fullpath, "%s/%s", p, "key.bin");

	f = fopen(fullpath, "r");
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

int key_create(gcry_sexp_t *key, const char *p) {
	FILE *f;
	const char *sexp_quick = "(genkey(ecc(curve Ed25519)))";
	char *pv;
	int i;
	int l;
	size_t c;
	gcry_sexp_t in;
	gcry_error_t e;
	char v[BUFLEN];

	e = gcry_sexp_new(&in, (const void*)sexp_quick, strlen(sexp_quick), 0);
	if (e) {
		printf("error sexp: %s\n", gcry_strerror(e));
		return (int)e;
	}
	e = gcry_pk_genkey(key, in);
	if (e) {
		printf("error gen: %s\n", gcry_strerror(e));
		return (int)e;
	}
	gcry_sexp_sprint(*key, GCRYSEXP_FMT_CANON, v, BUFLEN);
	f = fopen(p, "w");
	if (f == NULL) {
		return ERR_KEYFAIL;
	}
	c = 1;
	i = 0;
	l = strlen(v);
	pv = v;
	while (c != 0 && l > 0) {
		c = fwrite(pv+i, 1024, 1, f);
		i += c;
		l -= c;
	}
	fclose(f);

	return ERR_OK;
}

int sign(gcry_sexp_t *out, gcry_sexp_t *key, const char *v) {
	gcry_error_t e;
	//size_t l;
	gcry_sexp_t data;
	char in[BUFLEN];

	sprintf(in, "(data(flags eddsa(hash-algo sha512(value %s))))", v);
	gcry_sexp_new(&data, in, strlen(in), 0);
	e = gcry_pk_sign(out, data, *key);
	if (e) {
		sprintf(in, "error sign: %s\n", gcry_strerror(e));
		debugLog(DEBUG_ERROR, in);
		return ERR_KEYFAIL;
	}
	return 0;
}

int GpgStore::check(std::string p) {
	int r;
	const char *v;
	char d[1024];
	gcry_sexp_t k;
	gcry_sexp_t o;
	unsigned char fingerprint[20] = { 0x00 };
	unsigned char fingerprint_hex[41] = { 0x00 };
	size_t fingerprint_len;

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
		char pp[1024];
		sprintf(pp, "%s/key.bin", p.c_str());
		r = key_create(&k, pp);
		if (r != ERR_OK) {
			return r;
		}
		gcry_pk_get_keygrip(k, fingerprint);
		bin_to_hex(fingerprint, 20, fingerprint_hex, &fingerprint_len);
		char ppp[2048];
		sprintf(ppp, "created key %s from %s", fingerprint_hex, pp);
		debugLog(DEBUG_INFO, ppp);
	} else {
		gcry_pk_get_keygrip(k, fingerprint);
		r = bin_to_hex(fingerprint, 20, fingerprint_hex, &fingerprint_len);
		char pp[4096];
		sprintf(pp, "found key %s in %s", fingerprint_hex, p.c_str());
		debugLog(DEBUG_INFO, pp);
	}
	r = sign(&o, &k, "foo");
	return r;
}
