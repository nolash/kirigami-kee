#include <lmdb.h>
#include <string>

enum DbKey {
	DbKeyCreditItem = 0x01,
};

class Db {
public:
	Db(std::string conn);
	int connect();
	~Db();
	int put(enum DbKey, char *data, size_t date_len);
private:
	const char *m_connstr;
	MDB_env *m_env;
};
