#include "db.h"


int main() {
	Db *db;
	char *d;
	char s[1024] = "db_XXXXXX";
	d = mkdtemp(s);

	db = new Db(d);
	return db->connect();
}
