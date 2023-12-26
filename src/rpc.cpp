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

int process_new_certificate(char *b, size_t b_len) {
	Import *im;
	const Credit *credit;
	std::ostringstream ss;
	const char *s;

	im = new Import(b, b_len);
	credit = new Credit(im);
	ss << credit;
	s = ss.str().c_str();
	debugLog(DEBUG_DEBUG, s);
	return 0;
}

void RpcSocket::incomingConnection(quintptr fd) {
	char cmd;
	char buf[1024];
	char unpacked[1024];
	size_t unpacked_len;
	char onetwo[2] = { 0 , 1};
	int c;
	size_t l;
	int r;
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
	cmd = buf[0];
	buf[c] = 0x0;
	unpacked_len = 1024;
	r = unpack(buf+1, strlen(buf+1), unpacked, &unpacked_len);
	if (r) {
		debugLog(DEBUG_ERROR, "command unpack failed");
		return;
	}

	switch(cmd){
		case 1:
			r = process_new_certificate(unpacked, unpacked_len);
			break;
		default:
			sock.write(onetwo+1, 1);
			sock.flush();
			sock.close();
			debugLog(DEBUG_ERROR, "unknown ipc command");
			return;
	}
	sock.write(onetwo, 1);
	sock.flush();
	sock.close();
	debugLog(DEBUG_TRACE, "processed ipc command");
}
