#include <sstream>
#include "backend.h"
#include "debug.h"
#include "export.h"
#include "credit.h"
#include "command.h"
#include "item.h"
extern "C" {
#include "transport.h"
}

#ifndef ISTESTING
static
#endif
int process_rpc_new_certificate(Backend *backend, char *b, size_t b_len, bool preview) {
	Import *im;
	Credit *credit;
	std::ostringstream ss;
	const char *s;

	im = new Import(b, b_len);
	credit = new Credit(im);
	ss << credit;
	s = ss.str().c_str();
	debugLog(DEBUG_DEBUG, s);
	if (!preview) {
		backend->add(0, (void*)credit);
	} else {
		backend->set_preview(credit->preview());
	}

	return 0;
}

int process_rpc_command(Backend *backend, char *buf, size_t buf_len, bool preview) {
	int r;
	char cmd;
	size_t unpacked_len;
	char unpacked[1024];

	r = unpack(buf, buf_len, unpacked, &unpacked_len);
	cmd = unpacked[0]; //buf[0];
	if (r) {
		debugLog(DEBUG_ERROR, "command unpack failed");
		return 1;
	}

	switch(cmd){
		case 1:
			r = process_rpc_new_certificate(backend, unpacked+1, unpacked_len-1, preview);
			break;
		default:
			debugLog(DEBUG_ERROR, "unknown ipc command");
			return 1;
	}
	debugLog(DEBUG_TRACE, "processed ipc command");
	return 0;
}

int preview_command(char *in, size_t in_len, char *out, size_t *out_len) {
	strcpy(out, "# foo\n\n## bar\n\nhey ho\n\n* one\n*two\n\n");
	*out_len = strlen(out);
	return 0;
}
