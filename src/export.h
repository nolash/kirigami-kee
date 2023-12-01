#ifndef _EXPORT_H
#define _EXPORT_H

#include <stdint.h>
#include <vector>


class Export {
public:
	Export(size_t count);
	~Export();
	int addItem(char *in, size_t in_len);
	int write(char *out, size_t out_len);
private:
	size_t m_cap;
	int m_count;
	size_t m_size;
	char *m_lens;
	std::vector<size_t> m_lenlens;
	std::vector<char*> m_data;
	std::vector<size_t> m_datalens;
};


class Import {
public:
	Import(const char *in, size_t in_len);
	int read(char *out, size_t out_len);
private:
	char *m_data;
	size_t m_size;
	unsigned int m_crsr;
};

#endif // _EXPORT_H
