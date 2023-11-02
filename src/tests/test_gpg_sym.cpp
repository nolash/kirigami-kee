#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "gpg.h"
#include "testutil.h"


int test_pad() {
	size_t r;

	r = get_padsize(11, 6);
	if (r != 12) {
		return 1;
	}

	r = get_padsize(12, 6);
	if (r != 18) {
		return 2;
	}

	r = get_padsize(6, 13);
	if (r != 13) {
		return 3;
	}

	return 0;
}

int test_crypt(char *key, char *nonce) {
	int r;
	size_t l;
	std::string plain_r;
	const char *plain;
	const char *data = "foo";
	size_t data_len = strlen(data);
	char cipher[42]= {0x00};

	l = get_padsize(data_len, 42);
	r = encrypt((char*)cipher, l, data, key, nonce);
	if (r) {
		return 1;
	}
	r = decrypt(&plain_r, (const char*)cipher, l, key, nonce);
	if (r) {
		return 2;
	}

	plain = plain_r.c_str();
	r = strcmp(plain, "foo");
	if (r) {
		return 3;
	}
	return 0;
}

int test_crypt_binary(char *key, char *nonce) {
	int r;
	size_t l;
	const char *data = "foo";
	size_t data_len = strlen(data) + 1;
	char cipher[42]= {0x00};
	char plain[42];

	l = get_padsize(data_len, 42);
	r = encryptb((char*)cipher, l, data, data_len, key, nonce);
	if (r) {
		return 1;
	}
	r = decryptb(plain, (const char*)cipher, l, key, nonce);
	if (r) {
		return 2;
	}

	r = strcmp(plain, "foo");
	if (r) {
		return 3;
	}
	return 0;

}

int main() {
	int r;
	char key[32];
	char nonce[12];

	r = testutil_init();
	r += testutil_get_rand(key, 32);
	r += testutil_get_rand(nonce, 12);
	if (r) {
		return r;
	}

	r = test_pad();
	if (r) {
		return r;
	}
	r = test_crypt(key, nonce);
	if (r) {
		r |= (1 << 8);
		return r;
	}
	r = test_crypt_binary(key, nonce);
	if (r) {
		r |= (1 << 9);
		return r;
	}
	return 0;
}
