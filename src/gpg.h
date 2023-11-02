#ifndef _KEE_GPG
#define _KEE_GPG
#include <string>
#include <stddef.h>

#define GPG_MIN_VERSION "1.10.2"
#define CHACHA20_KEY_LENGTH_BYTES 32
#define CHACHA20_NONCE_LENGTH_BYTES 12
#define ENCRYPT_BLOCKSIZE 4096

int encrypt(char *ciphertext, size_t ciphertext_len, std::string keydata, const char *key, const char *nonce);
int encryptb (char *ciphertext, size_t ciphertext_len, const char *keydata, size_t keydata_len, const char *key, const char *nonce);
int decrypt(std::string *keydata, const char *ciphertext, size_t ciphertext_len, const char *key, const char *nonce);
int decryptb(char *keydata, const char *ciphertext, size_t ciphertext_len, const char *key, const char *nonce);
size_t get_padsize(size_t insize, size_t blocksize);

class GpgStore {

	public:
		int check(std::string p, std::string passphrase);
		char *get_fingerprint();
	
	private:
		const char *m_version;
		char *m_seckey;
		char m_fingerprint[41];
};
#endif
