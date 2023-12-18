#include <QLocalServer>

class RpcSocket : public QLocalServer {
	Q_OBJECT

protected:
	void incomingConnection(quintptr fd) override;
};
