#include "db.h"


int main() {
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
}
