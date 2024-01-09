#include <stdio.h>
#include "debug.h"
#include "gpg.h"


/// test opening existing keystore
int main() {
	int r;
	GpgStore *gpg;
       
	gpg = new GpgStore();
	r = gpg->check(".", "1234");
	return r;
}
