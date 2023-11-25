#include <fcntl.h>
#include <string>
#include <lmdb.h>
#include <gcrypt.h>
#include <time.h>

#include "db.h"
#include "err.h"


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

int Db::put(enum DbKey skey, char *data, size_t data_len) {
	int r;
	char *buf;
	unsigned char *rv;
	char kv;
	struct timespec ts;
	gcry_error_t e;
	gcry_md_hd_t h;
	MDB_txn *tx;
	MDB_dbi dbi;
	MDB_val k;
	MDB_val v;

	buf = (char*)malloc(1 + 32 + sizeof(struct timespec)); // length should be lookup in sizes array for each key

	r = clock_gettime(CLOCK_REALTIME, &ts);
	if (r) {
		return 1;
	}
	e = gcry_md_open(&h, GCRY_MD_SHA256, 0);
	if (e) {
		return ERR_DIGESTFAIL;
	}
	gcry_md_write(h, data, data_len);
	rv = gcry_md_read(h, 0);
	kv = (char)skey;
	memcpy(buf, &kv, 1);
	memcpy(buf + 1, &ts, sizeof(struct timespec));
	memcpy(buf + 1 + sizeof(struct timespec), rv, 32);
	gcry_md_close(h);

	r = mdb_txn_begin(m_env, NULL, 0, &tx);
	if (r) {
		return 1;
	}

	r = mdb_dbi_open(tx, NULL, MDB_CREATE, &dbi);
	if (r) {
		return 1;
	}

	k.mv_data = buf;
	k.mv_size = 1 + 32 + sizeof(struct timespec);
	v.mv_data = data;
	v.mv_size = data_len;

	r = mdb_put(tx, dbi, &k, &v, 0);
	if (r) {
		return 1;
	}

	r = mdb_txn_commit(tx);
	if (r) {
		return 1;
	}

	return 0;
}
