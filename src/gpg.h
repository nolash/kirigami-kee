#ifndef _KEE_GPG
#define _KEE_GPG
#include <string>
#include <stddef.h>


int encrypt(char *ciphertext, size_t ciphertext_len, std::string keydata, const char *key, const char *nonce);
int decrypt(std::string *keydata, const char *ciphertext, size_t ciphertext_len, const char *key, const char *nonce);

class GpgStore {

	public:
		int check(std::string p);
	
	private:
		const char *m_version;
};
#endif
