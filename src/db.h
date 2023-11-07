#include <lmdb.h>
#include <string>


class Db {
public:
	Db(std::string conn);
	int connect();
	~Db();
private:
	const char *m_connstr;
	MDB_env *m_env;
};
