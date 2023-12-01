#include <fcntl.h>
#include <string>
#include <lmdb.h>
#include <gcrypt.h>
#include <time.h>

#include "db.h"
#include "err.h"
#include "endian.h"


Db::Db(std::string conn) {
	m_connstr = conn;
	m_current_key = DbNoKey;
	m_tx = NULL;
	m_env = NULL;
	m_crsr = NULL;
	m_started = 0;
}

int Db::connect() {
	int r;

	r = mdb_env_create(&m_env);
	if (r) {
		return 1;
	}
	r = mdb_env_open(m_env, m_connstr.c_str(), MDB_NOLOCK, S_IRWXU);
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
	char rts[sizeof(struct timespec)];
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
	memcpy(rts, &ts.tv_sec, sizeof(ts.tv_sec));
	memcpy(rts + sizeof(ts.tv_sec), &ts.tv_nsec, sizeof(ts.tv_nsec));
	to_endian(0, sizeof(ts.tv_sec), rts);
	to_endian(0, sizeof(ts.tv_nsec), rts + sizeof(ts.tv_sec));

	e = gcry_md_open(&h, GCRY_MD_SHA256, 0);
	if (e) {
		return ERR_DIGESTFAIL;
	}
	gcry_md_write(h, data, data_len);
	rv = gcry_md_read(h, 0);
	kv = (char)skey;
	memcpy(buf, &kv, 1);
	memcpy(buf + 1, rts, sizeof(struct timespec));
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

int Db::next (enum DbKey skey, char **key, size_t *key_len, char **value, size_t *value_len) {
	int r;
	char start;
	MDB_val k;
	MDB_val v;

	if (skey != m_current_key) {
		if (m_started) {
			mdb_cursor_close(m_crsr);
			mdb_dbi_close(m_env, m_dbi);
			mdb_txn_abort(m_tx);
		}


		r = mdb_txn_begin(m_env, NULL, MDB_RDONLY, &m_tx);
		if (r) {
			return 1;
		}
		r = mdb_dbi_open(m_tx, NULL, 0, &m_dbi);
		if (r) {
			return 1;
		}
		
		r = mdb_cursor_open(m_tx, m_dbi, &m_crsr);	
		if (r) {
			return 1;
		}
		m_current_key = skey;

		start = (char)skey;
		k.mv_data = &start;
		k.mv_size = 1;
	}


	r = mdb_cursor_get(m_crsr, &k, &v, MDB_NEXT);
	if (r) {
		return 1;
	}

	*key = (char*)k.mv_data;
	*key_len = k.mv_size;
	*value = (char*)v.mv_data;
	*value_len = v.mv_size;

	return 0;
}
