#ifndef _EXPORT_H
#define _EXPORT_H

#include <stdint.h>
#include <vector>


/**
 *
 * \brief Provides a buffer for serializing an given amount of data fields for storage.
 *
 * Serialized data is a concatenated string of varint and data pairs, where the varints specify the following data length.
 *
 * An instance should only be used once for every serialization.
 *
 * \todo Define meta-class
 *
 */
class Export {
public:
	/**
	 *
	 * \brief Declare the amount of data fields that the instance will serialize.
	 *
	 * \param count Number of fields
	 *
	 */
	Export(size_t count);
	~Export();
	/**
	 * 
	 * \brief Add a single item to the serialization buffer.
	 *
	 * Must be called same amount of times as the \c count given in the constructor.
	 *
	 * \param in Data bytes to insert.
	 * \param in_len Length of data bytes.
	 * \return ERR_OK if successful, ERR_FAIL if the entry count has been exceeded.
	 *
	 */
	int addItem(char *in, size_t in_len);
	/**
	 *
	 * \brief Write the serialized data to the provided buffer.
	 *
	 * May be called any number of times, when successful, the result will always be the same.
	 *
	 * \param out If successful, will contain the serialized data
	 * \param out_len The available
	 * \return -1 if entries are missing, or a positive number indicated the number of bytes written on success.
	 */
	int write(char *out, size_t out_len);
private:
	/// Declared entry count
	size_t m_cap;
	/// Current entry count
	int m_count;
	/// Size of data currently written
	size_t m_size;
	/// Concatenated string of varint lengths
	char *m_lens;
	/// Size of each varint length in m_lens
	std::vector<size_t> m_lenlens;
	/// List of entry data
	std::vector<char*> m_data;
	/// Lengths of entry data
	std::vector<size_t> m_datalens;
};


/**
 *
 * \brief Provides a buffer for deserializing a data object.
 *
 * An instance should only be used once for every deserialization.
 *
 * The instance does no checking whether the deserialized data fits the requirements of the class that uses it.
 *
 * \todo define meta-class.
 *
 */
class Import {
public:
	/**
	 *
	 * \brief Provide the input data buffer to import.
	 *
	 * \param in Input data.
	 * \param in_len Length of input data.
	 *
	 */
	Import(const char *in, size_t in_len);
	/**
	 *
	 * \brief Read a single data field from the deserialized entry.
	 *
	 * \param out If successful, will contain the deserialized data field.
	 * \param out_len Length of deserialized data field.
	 * \return Positive number indicating the number of bytes read, or 0 if no more data is available.
	 */
	int read(char *out, size_t out_len);
	/**
	 *
	 * \brief Check whether deserialization is completed.
	 *
	 * \return true (1) if done.
	 *
	 */
	bool done();
private:
	/// Cached input data.
	char *m_data;
	/// Input data size.
	size_t m_size;
	/// End byte of last successful read.
	unsigned int m_crsr;
	/// True if read has been attempted when no more available data.
	bool m_eof;
};

#endif // _EXPORT_H
