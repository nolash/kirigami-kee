#include <QLocalSocket>
#include "rpc.h"


void RpcSocket::incomingConnection(quintptr fd) {
	char buf[1024];
	int c;
	bool r;
	QLocalSocket sock;
	
	r = sock.setSocketDescriptor(fd);
	if (!r) {
		return;
	}

	sock.waitForReadyRead();
	sock.read(buf, 1024);
	qDebug() << buf;
}
