#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "gpg.h"
#include "testutil.h"

int main() {
	int r;
	char key[32];
	char nonce[12];
	char cipher[1024]= {0x00};
	std::string plain_r;
	const char *plain;

	r = testutil_init();
	r += testutil_get_rand(key, 32);
	r += testutil_get_rand(nonce, 12);
	if (r) {
		return r;
	}

	r = encrypt((char*)cipher, 1024, "foo", key, nonce);
	if (r) {
		return r;
	}
	r = decrypt(&plain_r, (const char*)cipher, 1024, key, nonce);
	if (r) {
		return r;
	}

	plain = plain_r.c_str();
	r = strcmp(plain, "foo");
	
	return r;
}
