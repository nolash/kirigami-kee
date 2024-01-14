#include <sstream>
#include <QLocalSocket>
#include "rpc.h"
#include "backend.h"
#include "debug.h"
#include "command.h"


RpcSocket::RpcSocket(Backend *backend) {
	m_backend = backend;
}


void RpcSocket::incomingConnection(quintptr fd) {
	char buf[RPC_COMMAND_SIZE];
	int c;
	size_t l;
	char r;
	bool rr;
	QLocalSocket sock;
	
	rr = sock.setSocketDescriptor(fd);
	if (!rr) {
		return;
	}

	sock.waitForReadyRead();
	l = 0;
	while (1) {
		c = sock.read(buf+l, RPC_BUFFER_SIZE);
		if (c == 0) {
			break;
		}
		l += c;
		if (l > RPC_COMMAND_SIZE) {
			debugLog(DEBUG_ERROR, "too long ipc command");
			return;
		}
	}
	if (l < 2) {
		debugLog(DEBUG_ERROR, "short ipc command");
		return;
	}

	r = 0;
	process_rpc_command(m_backend, buf, l, &r);
	sock.write(&r, 1);
	sock.flush();
	sock.close();
}
