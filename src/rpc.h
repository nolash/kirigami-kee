#include <QLocalServer>
#include "backend.h"


class RpcSocket : public QLocalServer {
	Q_OBJECT

public:
	RpcSocket(Backend *b);
protected:
	void incomingConnection(quintptr fd) override;
private:
	Backend *m_backend;
};
