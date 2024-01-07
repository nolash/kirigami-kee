#ifndef _DB_H
#define _DB_H

#include <lmdb.h>
#include <string>

enum DbKey {
	DbNoKey = 0x00,
	DbKeyCreditItem = 0x01,
	DbKeyReverse = 0xff,
};

class Db {
public:
	Db(std::string conn);
	int connect();
	~Db();
	int put(enum DbKey, char *data, size_t date_len);
	int next (enum DbKey skey, char **key, size_t *key_len, char **value, size_t *value_len);
private:
	std::string m_connstr;
	enum DbKey m_current_key;
	MDB_env *m_env;
	MDB_dbi m_dbi;
	MDB_txn *m_tx;
	MDB_cursor *m_crsr;
	bool m_started;
};

#endif // _DB_H
