#include <sstream>
#include <QLocalSocket>
#include "rpc.h"
#include "backend.h"
#include "debug.h"
#include "export.h"
#include "credit.h"
extern "C" {
#include "transport.h"
}


RpcSocket::RpcSocket(Backend *backend) {
	m_backend = backend;
}

int process_rpc_new_certificate(Backend *backend, char *b, size_t b_len) {
	Import *im;
	const Credit *credit;
	std::ostringstream ss;
	const char *s;

	im = new Import(b, b_len);
	credit = new Credit(im);
	ss << credit;
	s = ss.str().c_str();
	debugLog(DEBUG_DEBUG, s);
	backend->add(0, (void*)credit);

	return 0;
}

int process_rpc_command(Backend *backend, char *buf, size_t buf_len, char *result) {
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
			r = process_rpc_new_certificate(backend, unpacked+1, unpacked_len-1);
			break;
		default:
			*result = 1;
			debugLog(DEBUG_ERROR, "unknown ipc command");
			return 1;
	}
	*result = 0;
	debugLog(DEBUG_TRACE, "processed ipc command");
	return 0;
}

void RpcSocket::incomingConnection(quintptr fd) {
	char buf[1024];
	int c;
	char r;
	bool rr;
	QLocalSocket sock;
	
	rr = sock.setSocketDescriptor(fd);
	if (!rr) {
		return;
	}

	sock.waitForReadyRead();
	c = sock.read(buf, 1024);
	if (c == 1) {
		debugLog(DEBUG_ERROR, "short ipc command");
		return;
	}

	r = 0;
	process_rpc_command(m_backend, buf, c, &r);
	sock.write(&r, 1);
	sock.flush();
	sock.close();
}
