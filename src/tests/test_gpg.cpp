#include <stdio.h>
#include "debug.h"
#include "gpg.h"


int main() {
	int r;
	GpgStore *gpg;
       
	gpg = new GpgStore();
	r = gpg->check("key.bin");
	return r;
}
