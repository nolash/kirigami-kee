#include <QLocalSocket>

#include "backend.h"
#include "rpc.h"
#include "test_rpc.h"


int process_rpc_command(Backend *backend, char *buf, size_t buf_len, char *result);

int RpcTest::connect_test(RpcSocket *rpc) {
	bool r_sock;
	qint64 r;
	QString s;
	char cmd[] = { 0x01, 0x01, 0x66, 0x02, 0x62, 0x61, 0x0a };
	QLocalSocket sock_out;
	char buf[1024];

	s = QString::fromUtf8("test.rpc");
	rpc->setSocketOptions(QLocalServer::UserAccessOption);
	r_sock = rpc->listen(s);
	if (!r_sock) {
		return 1;
	}

	s = rpc->fullServerName();
	m_socket = new QLocalSocket(this);
	m_socket->connectToServer(s);
	if (!m_socket->waitForConnected()) {
		return 1;
	}

	sock_out.setSocketDescriptor(rpc->socketDescriptor());
	if (!sock_out.isValid()) {
		return 1;
	}

	if (!rpc->hasPendingConnections()) {
		return 1;
	}

	r = m_socket->write(cmd, 7);
	if (r != 7) {
		return 1;
	}
	m_socket->flush();

	if (!sock_out.waitForReadyRead(10000)) {
		return 1;
	}
	r = sock_out.read(buf, 1024);
	if (r == 0) {
		return 1;
	}
	m_socket->close();
	return 0;
}

int test_processor() {
	int r;
	char r_rpc;
	Backend backend;
	int c;
	CreditListModel credit_model;
	char data[] = "eAFjZEnLz09iTkosAgANWwLk"; // 0x0104666f6f6203626172

	backend.set_credit_list(&credit_model);

	c = strlen(data) + 1;
	r = process_rpc_command(&backend, data, c, &r_rpc);

	return r;
}

/**
 * \todo doesnt receive from server socket, probably needs threads
 */
int test_socket() {
	int r;

	RpcTest *rpc_test;
	QString sock_name;
	Backend backend;
	RpcSocket *rpc = new RpcSocket(&backend);

	rpc_test = new RpcTest();
	r = rpc_test->connect_test(rpc);
	if (r) {
		return 1;
	}

	return 0;
}

int main() {
	int r;

	r = test_processor();
	if (r) {
		return r;
	}

	//r = test_socket();
	//return r;
	//
	return 0;
}
