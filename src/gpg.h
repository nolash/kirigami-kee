#ifndef _KEE_GPG
#define _KEE_GPG
#include <string>
#include <stddef.h>

#define GPG_MIN_VERSION "1.10.2"
#define CHACHA20_KEY_LENGTH_BYTES 32
#define CHACHA20_NONCE_LENGTH_BYTES 12
#define ENCRYPT_PADSIZE 256 * 1024

int encrypt(char *ciphertext, size_t ciphertext_len, std::string keydata, const char *key, const char *nonce);
int decrypt(std::string *keydata, const char *ciphertext, size_t ciphertext_len, const char *key, const char *nonce);

class GpgStore {

	public:
		int check(std::string p);
	
	private:
		const char *m_version;
};
#endif
