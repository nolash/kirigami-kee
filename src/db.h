#ifndef _DB_H
#define _DB_H

#include <lmdb.h>
#include <string>

#ifndef DB_KEY_SIZE_LIMIT
#define DB_KEY_SIZE_LIMIT 64
#endif

#ifndef DB_VALUE_SIZE_LIMIT 
#define DB_VALUE_SIZE_LIMIT 1048576
#endif

/**
 * \brief Key prefixes used for database storage.
 *
 */
enum DbKey {
	/// Noop value, used for default value of a DbKey variable
	DbNoKey = 0x00,
	/// A credit item record
	DbKeyCreditItem = 0x01,
	/// A reverse lookup record; resolves the content hash to the content entry in the database.
	DbKeyReverse = 0xff,
};

/**
 *
 * \brief Interface to persistent storage of data items used in application.
 *
 */
class Db {
public:
	/**
	 * \brief Database constructor.
	 *
	 * Database instance cannot be used until \c connect() has been successfully called.
	 *
	 * \param conn Database connection string.
	 *
	 */
	Db(std::string conn);
	/**
	 *
	 * \brief Connect to and verify connection to database.
	 *
	 * \return ERR_OK if successful, ERR_FAIL if not.
	 *
	 */
	int connect();
	/**
	 *
	 * \brief Safely disconnect from database.
	 *
	 */
	~Db();
	/**
	 *
	 * \brief Add data item to database.
	 *
	 * Entry will be filed under the concatenation of the prefix, the current realtime clock value (struct timespec) and the sha256 hash of the value.
	 *
	 * A reverse index entry will also be added, prefixed by DbKeyReverse concatenated with the sha256 hash, and with the entry key as value.
	 *
	 * No checks are performed to validate the data against the data type specified by the prefix.
	 *
	 * \param pfx Prefix to use for entry.
	 * \param data Data to add.
	 * \param data_len Length of data to add.
	 * \return ERR_OK if successful, ERR_FAIL if not.
	 *
	 */
	int put(enum DbKey pfx, char *data, size_t data_len);
	/**
	 *
	 * \brief Get the entry matching the given prefix and key segment.
	 *
	 * If an exact match is not found, the closest next match (in big-endian byte order) with the same prefix will be used.
	 *
	 * If no match is found, \c reset() will be called automatically.
	 *
	 * The iteration is not thread safe.
	 *
	 * \param pfx Key prefix to search for. If key, 
	 * \param key Optional key segment to search for. If call is successful, points to the key found.
	 * \param key_len Optional length of key segment to search for (excluding prefix). If call is successful, points to the byte length of the key found.
	 * \param value If call is successful, points to the value of the entry for the key found.
	 * \param value_len If call is successful, points to the byte length of the value of the entry.
	 * \return ERR_OK on success, ERR_DB_NOMATCH if no match is found, ERR_DB_INVALID if the request is invalid, or ERR_DB_FAIL for any other error.
	 *
	 */
	int next (enum DbKey pfx, char **key, size_t *key_len, char **value, size_t *value_len);
	/**
	 *
	 * \brief Clear the stored key match, preparing for a new search.
	 *
	 */
	void reset();
private:
	std::string m_connstr;
	enum DbKey m_current_key;
	MDB_env *m_env;
	MDB_dbi m_dbi;
	MDB_txn *m_tx;
	MDB_cursor *m_crsr;
	MDB_val m_k;
	MDB_val m_v;
	bool m_started;
	bool m_browsing;
};

#endif // _DB_H
