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
	m_tx = NULL;
	m_env = NULL;
	m_crsr = NULL;
	m_started = 0;
	m_browsing = 0;
	this->reset();
}

int Db::connect() {
	int r;

	r = mdb_env_create(&m_env);
	if (r) {
		return ERR_FAIL;
	}
	r = mdb_env_open(m_env, m_connstr.c_str(), MDB_NOLOCK, S_IRWXU);
	if (r) {
		return ERR_FAIL;
	}


	return ERR_OK;
}

Db::~Db() {
	mdb_env_close(m_env);
}

/**
 * \todo split up and optimize
 */
int Db::put(enum DbKey pfx, char *data, size_t data_len) {
	int r;
	char *buf;
	char buf_reverse[33];
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
		return ERR_FAIL;
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
	kv = (char)pfx;
	memcpy(buf, &kv, 1);
	memcpy(buf + 1, rts, sizeof(struct timespec));
	memcpy(buf + 1 + sizeof(struct timespec), rv, 32);

	r = mdb_txn_begin(m_env, NULL, 0, &tx);
	if (r) {
		return ERR_FAIL;
	}

	r = mdb_dbi_open(tx, NULL, MDB_CREATE, &dbi);
	if (r) {
		return ERR_FAIL;
	}

	k.mv_data = buf;
	k.mv_size = 1 + 32 + sizeof(struct timespec);
	v.mv_data = data;
	v.mv_size = data_len;

	r = mdb_put(tx, dbi, &k, &v, 0);
	if (r) {
		return ERR_FAIL;
	}

	// put reverse lookup
	buf_reverse[0] = (char)DbKeyReverse;
	memcpy(buf_reverse+1, rv, 32);
	k.mv_data = buf_reverse; 
	k.mv_size = 33;
	v.mv_data = buf;
	v.mv_size = 1 + 32 + sizeof(struct timespec);
	gcry_md_close(h); // keep the handle open until here because we use its digest value again for the key

	r = mdb_put(tx, dbi, &k, &v, 0);
	if (r) {
		return ERR_FAIL;
	}

	r = mdb_txn_commit(tx);
	if (r) {
		return ERR_FAIL;
	}

	return ERR_OK;
}

/**
 *
 * \todo change cursor to jump to new search match when current (last) prefix does not match lookup prefix.
 *
 */
int Db::next (enum DbKey pfx, char **key, size_t *key_len, char **value, size_t *value_len) {
	int r;
	unsigned char start[DB_KEY_SIZE_LIMIT];

	memset(start, 0, DB_KEY_SIZE_LIMIT);;
	if ((char)pfx == 0) {
		return ERR_DB_INVALID;
	}

	if (m_current_key == DbNoKey) {
		if (m_started) {
			mdb_cursor_close(m_crsr);
			mdb_dbi_close(m_env, m_dbi);
			mdb_txn_abort(m_tx);
		}

		r = mdb_txn_begin(m_env, NULL, MDB_RDONLY, &m_tx);
		if (r) {
			return ERR_DB_FAIL;
		}
		r = mdb_dbi_open(m_tx, NULL, 0, &m_dbi);
		if (r) {
			return ERR_DB_FAIL;
		}
		
		r = mdb_cursor_open(m_tx, m_dbi, &m_crsr);	
		if (r) {
			return ERR_DB_FAIL;
		}
		m_current_key = pfx;

		start[0] = (char)pfx;
		m_k.mv_size = 1;
		if (!m_browsing) {
			if (*key != 0) {
				memcpy(start+1, *key, *key_len);
				m_k.mv_size += *key_len;
			}
		}
		m_k.mv_data = start;
	}

	if (!m_browsing) {
		r = mdb_cursor_get(m_crsr, &m_k, &m_v, MDB_SET_RANGE);
		m_browsing = 1;
	} else {
		r = mdb_cursor_get(m_crsr, &m_k, &m_v, MDB_NEXT_NODUP);
	}
	if (r) {
		return ERR_DB_FAIL;
	}
	start[0] = (char)*((char*)m_k.mv_data);
	if (start[0] != m_current_key) {
		reset();
		return ERR_DB_NOMATCH;
	}

	*key = (char*)m_k.mv_data;
	*key_len = m_k.mv_size;
	*value = (char*)m_v.mv_data;
	*value_len = m_v.mv_size;

	return 0;
}

void Db::reset() {
	m_k.mv_data = 0x0;
	m_k.mv_size = 0;
	m_current_key = DbNoKey;
	m_browsing = 0;
}
