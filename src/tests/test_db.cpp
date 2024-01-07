#include <string.h>

#include "db.h"
#include <time.h>



int test_put() {
	int r;
	Db *db;
	char *d;
	char s[1024] = "db_XXXXXX";
	char data[] = "foo";
	d = mkdtemp(s);

	db = new Db(d);
	r = db->connect();
	if (r) {
		return 1;
	}
	
	r = db->put(DbKeyCreditItem, data, 3);
	if (r) {
		return 1;
	}

	return 0;
}

int test_iter() {
	int r;
	Db *db;
	char *d;
	char s[1024] = "db_XXXXXX";
	char data[64];
	char *k;
	char *v;
	size_t kl;
	size_t vl;
	char kc[33];
	d = mkdtemp(s);

	db = new Db(d);
	r = db->connect();
	if (r) {
		return 1;
	}

	strcpy(data, "foo");	
	r = db->put(DbKeyCreditItem, data, 3);
	if (r) {
		return 1;
	}

	strcpy(data, "barbaz");	
	r = db->put(DbKeyCreditItem, data, 6);
	if (r) {
		return 1;
	}

	r = db->next(DbKeyCreditItem, &k, &kl, &v, &vl);
	if (r) {
		return 1;
	}

	r = memcmp(v, "foo", vl);
	if (r) {
		return 1;
	}
	kc[0] = 0xff;
	memcpy(kc + 1, k + 1 + sizeof(struct timespec), 32);

	r = db->next(DbKeyCreditItem, &k, &kl, &v, &vl);
	if (r) {
		return 1;
	}

	r = memcmp(v, "barbaz", vl);
	if (r) {
		return 1;
	}

	r = db->next(DbKeyCreditItem, &k, &kl, &v, &vl);
	if (r) {
		return 1;
	}

	r = memcmp(k, kc, 33);
	if (r) {
		return 1;
	}

	return 0;
}

int main() {
	int r;

	r = test_put();
	if (r) {
		return 1;
	}

	r = test_iter();
	if (r) {
		return 1;
	}

	return 0;
}
