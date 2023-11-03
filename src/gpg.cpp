#include <gcrypt.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>

#include "err.h"
#include "debug.h"
#include "gpg.h"
#include "hex.h"

#define BUFLEN 1024 * 1024

const char *gpgVersion = nullptr;


size_t get_padsize(size_t insize, size_t blocksize) {
	size_t c;
	size_t l;
	size_t m;

	c = insize + 1;
	l = c / blocksize;
	m = c % blocksize;
	if (m) {
		l++;
	}
	return l * blocksize;
}

void padb(char *data, size_t outsize, size_t insize) {
	gcry_randomize(data + insize, outsize - insize, GCRY_STRONG_RANDOM);
}

//TODO: explicit strlen
void pad(char *keydata_raw, size_t outsize, std::string keydata) {
	int l;

	strcpy(keydata_raw, keydata.c_str());
	l = keydata.length() + 1;
	padb(keydata_raw, outsize, l);
}

int create_handle(gcry_cipher_hd_t *h, const char *key, const char *nonce) {
	gcry_error_t e;

	e = gcry_cipher_open(h, GCRY_CIPHER_CHACHA20, GCRY_CIPHER_MODE_POLY1305, GCRY_CIPHER_SECURE);
	if (e) {
		return ERR_NOCRYPTO;
	}
	e = gcry_cipher_setkey(*h, key, CHACHA20_KEY_LENGTH_BYTES);
	if (e) {
		return ERR_NOCRYPTO;
	}
	e = gcry_cipher_setiv(*h, nonce, CHACHA20_NONCE_LENGTH_BYTES);
	if (e) {
		return ERR_NOCRYPTO;
	}
	return ERR_OK;
}


void free_handle(gcry_cipher_hd_t *h) {
	gcry_cipher_close(*h);
}

int encryptb (char *ciphertext, size_t ciphertext_len, const char *keydata, size_t keydata_len, const char *key, const char *nonce) {
	int r;
	gcry_cipher_hd_t h;
	gcry_error_t e;
	char keydata_raw[ciphertext_len];

	r = create_handle(&h, key, nonce);
	if (r) {
		return r;
	}
	memcpy(keydata_raw, keydata, keydata_len);
	padb(keydata_raw, ciphertext_len, keydata_len);
	e = gcry_cipher_encrypt(h, (unsigned char*)ciphertext, ciphertext_len, (const unsigned char*)keydata_raw, ciphertext_len);
	if (e) {
		return ERR_NOCRYPTO;
	}

	free_handle(&h);

	return ERR_OK;
}

int encrypt(char *ciphertext, size_t ciphertext_len, std::string keydata, const char *key, const char *nonce) {
	int r;
	gcry_cipher_hd_t h;
	gcry_error_t e;
	char keydata_raw[ciphertext_len];

	r = create_handle(&h, key, nonce);
	if (r) {
		return r;
	}

	pad(keydata_raw, ciphertext_len, keydata);
	e = gcry_cipher_encrypt(h, (unsigned char*)ciphertext, ciphertext_len, (const unsigned char*)keydata_raw, ciphertext_len);
	if (e) {
		return ERR_NOCRYPTO;
	}

	free_handle(&h);

	return ERR_OK;
}

int decryptb(char *keydata, const char *ciphertext, size_t ciphertext_len, const char *key, const char *nonce) {
	int r;
	gcry_cipher_hd_t h;
	gcry_error_t e;

	r = create_handle(&h, key, nonce);
	if (r) {
		return r;
	}

	e = gcry_cipher_decrypt(h, keydata, ciphertext_len, ciphertext, ciphertext_len);
	if (e) {
		return ERR_NOCRYPTO;
	}

	free_handle(&h);

	return ERR_OK;

}

int decrypt(std::string *keydata, const char *ciphertext, size_t ciphertext_len, const char *key, const char *nonce) {
	int r;
	gcry_cipher_hd_t h;
	gcry_error_t e;
	char keydata_raw[ciphertext_len] = {0x00};

	r = create_handle(&h, key, nonce);
	if (r) {
		return r;
	}

	e = gcry_cipher_decrypt(h, keydata_raw, ciphertext_len, ciphertext, ciphertext_len);
	if (e) {
		return ERR_NOCRYPTO;
	}
	keydata->assign(keydata_raw);

	free_handle(&h);

	return ERR_OK;
}

int key_from_data(gcry_sexp_t *key, const char *keydata, size_t keydata_len) {
	gcry_error_t e;

	e = gcry_sexp_new(key, keydata, keydata_len, 1);
	if (e != GPG_ERR_NO_ERROR) {
		//debugLog(DEBUG_DEBUG, keydata);
		return ERR_KEYFAIL;
	}
	return ERR_OK;
}

int key_from_path(gcry_sexp_t *key, const char *p, const char *passphrase) {
	int l;
	int r;
	char v[BUFLEN];
	size_t c;
	size_t i;
	FILE *f;
	char *fullpath;
	char nonce[CHACHA20_NONCE_LENGTH_BYTES];
	void *keydata;

	fullpath = (char*)malloc(strlen(p) + 8);
	sprintf(fullpath, "%s/%s", p, "key.bin");

	f = fopen(fullpath, "r");
	if (f == NULL) {
		return ERR_NOKEY;
	}
	free(fullpath);

	c = fread(&l, sizeof(int), 1, f);
	c = fread(nonce, CHACHA20_NONCE_LENGTH_BYTES, 1, f);
	i = 0;
	c = 1;
	while (c != 0 && i < BUFLEN) {
		c = fread(v+i, 1024, 1, f);
		c *= 1024;
		i += c;
	}
	if (i == 0) {
		return ERR_NOKEY;
	}
	fclose(f);

	keydata = malloc(i);
	r = decryptb((char*)keydata, v, i, passphrase, nonce);
	if (r) {
		return ERR_NOKEY;
	}
	r = key_from_data(key, (char*)keydata, l);
	free(keydata);
	return r;
}

int key_create(gcry_sexp_t *key, const char *p, const char *passphrase) {
	int r;
	FILE *f;
	const char *sexp_quick = "(genkey(ecc(curve Ed25519)))";
	char *pv;
	int i;
	int l;
	int kl;
	size_t c;
	gcry_sexp_t in;
	gcry_error_t e;
	char v[BUFLEN];
	char nonce[CHACHA20_NONCE_LENGTH_BYTES];


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
	kl = gcry_sexp_sprint(*key, GCRYSEXP_FMT_CANON, v, BUFLEN);

	// create padding
	c = get_padsize(kl, ENCRYPT_BLOCKSIZE);
	char ciphertext[c];

	gcry_create_nonce(nonce, CHACHA20_NONCE_LENGTH_BYTES);
	r = encryptb(ciphertext, c, v, kl, passphrase, nonce);
	if (r) {
		return ERR_KEYFAIL;
	}

	f = fopen(p, "w");
	if (f == NULL) {
		return ERR_KEYFAIL;
	}
	// TODO rename c
	l = c;
	// TODO: endian
	c = fwrite(&kl, sizeof(int), 1, f);
	if (c != 1) {
		fclose(f);
		return ERR_KEYFAIL;
	}
	c = fwrite(nonce, CHACHA20_NONCE_LENGTH_BYTES, 1, f);
	if (c != 1) {
		fclose(f);
		return ERR_KEYFAIL;
	}
	i = 0;
	c = 1;
	while (c != 0 && l > 0) {
		c = fwrite(ciphertext+i, 1024, 1, f);
		c *= 1024;
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

char *GpgStore::get_fingerprint() {
	return m_fingerprint;
}


int GpgStore::digest(char *buf, std::string in) {
	gcry_error_t e;
	gcry_md_hd_t h;
	unsigned char *v;

	e = gcry_md_open(&h, GCRY_MD_SHA256, GCRY_MD_FLAG_SECURE);
	if (e) {
		return ERR_DIGESTFAIL;
	}

	v = gcry_md_read(h, 0);
	memcpy(buf, v, m_passphrase_digest_len);
	gcry_md_write(h, in.c_str(), in.length());
	gcry_md_close(h);
	return ERR_OK;
}


GpgStore::GpgStore() {
	m_passphrase_digest_len = gcry_md_get_algo_dlen(GCRY_MD_SHA256);
}

int GpgStore::check(std::string p, std::string passphrase) {
	int r;
	const char *v;
	char d[1024];
	gcry_sexp_t k;
	gcry_sexp_t o;
	unsigned char fingerprint[20] = { 0x00 };
	size_t fingerprint_len = 41;
	char passphrase_hash[m_passphrase_digest_len];

	digest(passphrase_hash, passphrase);

	if (gpgVersion == nullptr) {
		v = gcry_check_version(GPG_MIN_VERSION);
		if (v == nullptr) {
			return ERR_NOCRYPTO;
		}
	}
	gpgVersion = v;
	sprintf(d, "Using gpg version: %s", gpgVersion);
	debugLog(DEBUG_INFO, d);
	//r = key_from_path(&k, p.c_str(), passphrase.c_str());
	r = key_from_path(&k, p.c_str(), passphrase_hash);
	if (r == ERR_KEYFAIL) {
		char pp[1024];
		sprintf(pp, "could not decrypt key in %s/key.bin", p.c_str());
		debugLog(DEBUG_CRITICAL, pp);
		return 1;
	}
	if (r != ERR_OK) {
		char pp[1024];
		sprintf(pp, "%s/key.bin", p.c_str());
		//r = key_create(&k, pp, passphrase.c_str());
		r = key_create(&k, pp, passphrase_hash);
		if (r != ERR_OK) {
			return r;
		}
		gcry_pk_get_keygrip(k, fingerprint);
		//bin_to_hex(fingerprint, 20, fingerprint_hex, &fingerprint_len);
		bin_to_hex(fingerprint, 20, (unsigned char*)m_fingerprint, &fingerprint_len);
		char ppp[2048];
		sprintf(ppp, "created key %s from %s", m_fingerprint, pp);
		debugLog(DEBUG_INFO, ppp);
	} else {
		gcry_pk_get_keygrip(k, fingerprint);
		//bin_to_hex(fingerprint, 20, fingerprint_hex, &fingerprint_len);
		bin_to_hex(fingerprint, 20, (unsigned char*)m_fingerprint, &fingerprint_len);
		char pp[4096];
		//sprintf(pp, "found key %s in %s", fingerprint_hex, p.c_str());
		sprintf(pp, "found key %s in %s", (unsigned char*)m_fingerprint, p.c_str());
		debugLog(DEBUG_INFO, pp);
	}
	r = sign(&o, &k, "foo");
	return r;
}
