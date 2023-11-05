#include <string.h>
#include <stdlib.h>

#include "export.h"

extern "C" {
	#include <varint.h>
}


Export::Export(size_t count)
	: m_cap(count) {
	m_lenlens.reserve(count);
	m_data.reserve(count);
	m_datalens.reserve(count);
	m_lens = (char*)malloc(10*count);
}

Export::~Export() {
	free(m_lens);
}

int Export::addItem(char *in, size_t in_len) {
	int r;
	char b[10];

	r = 0;
	r += varint_write_u(b, in_len);
	memcpy(m_lens+(m_count*10), b, 10);
	m_lenlens.push_back((size_t)r);
	m_datalens.push_back(in_len);
	m_data.push_back(in);

	m_size += r;
	m_count++;
	return r;
}

int Export::write(char *out, size_t out_len) {
	char *p;
	int i;
	size_t l;
	size_t c;

	if (out_len < m_size) {
		return -1;
	}

	p = out;
	c = 0;
	for (i = 0; i < m_count; i++) {
		l = m_lenlens[i];
		memcpy(p, m_lens+(10*i), l);
		p += l;
		c += l;
		l = m_datalens[i];
		memcpy(p, m_data[i], l);
		p += l;
		c += l;
	}
	return c;
}

Import::Import(const char *in, size_t in_len) 
	: m_data((char*)in), m_size(in_len) {
	m_crsr = 0;
}

int Import::read(char *out, size_t out_len) {
	int r;
	uint64_t l;

	if (m_size - m_crsr <= 0) {
		return 0;
	}

	r = varint_read_u(m_data + m_crsr, out_len, &l);
	m_crsr += r;

	memcpy(out, m_data + m_crsr, (size_t)l);
	m_crsr += l;

	return r;
}
