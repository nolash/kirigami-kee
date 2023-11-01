#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "gpg.h"


int main() {
	int r;
	GpgStore *gpg;
	char *d;
	char s[1024] = "nokey_XXXXXX";
	 
	d = mkdtemp(s);
       
	gpg = new GpgStore();
	r = gpg->check(d);
	return r;
}
