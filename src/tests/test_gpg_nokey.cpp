#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "gpg.h"
#include "sys/stat.h"


int main() {
	int r;
	GpgStore *gpg;
	char *d;
	char s[1024] = "nokey_XXXXXX";
	 
	d = mkdtemp(s);
       
	gpg = new GpgStore();
	r = gpg->check(d, "1234567890abcdef1234567890abcdef");
	if (r) {
		return r;
	}
	r = gpg->check(d, "1234567890abcdef1234567890abcdef");

	return r;
}
