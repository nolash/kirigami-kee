#include <fcntl.h>
#include <lmdb.h>
#include "db.h"
#include <string>


Db::Db(std::string conn) {
	m_connstr = conn.c_str();
}

int Db::connect() {
	int r;

	r = mdb_env_create(&m_env);
	if (r) {
		return 1;
	}
	r = mdb_env_open(m_env, m_connstr, MDB_NOLOCK, S_IRWXU);
	if (r) {
		return 1;
	}
	return 0;
}

Db::~Db() {
	mdb_env_close(m_env);
}
