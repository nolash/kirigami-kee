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

	cmd = buf[0];
	buf[buf_len] = 0x0;
	unpacked_len = 1024;
	//unpacked_len = buf_len;
	r = unpack(buf+1, strlen(buf+1), unpacked, &unpacked_len);
	if (r) {
		debugLog(DEBUG_ERROR, "command unpack failed");
		return 1;
	}

	switch(cmd){
		case 1:
			r = process_rpc_new_certificate(backend, unpacked, unpacked_len);
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
	int r;
	char r_cmd;
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

	r_cmd = 0;
	r = process_rpc_command(m_backend, buf, c, &r_cmd);
	sock.write(&r_cmd, 1);
	sock.flush();
	sock.close();
}

