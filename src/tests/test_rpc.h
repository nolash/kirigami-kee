#include "rpc.h"


class RpcTest : public QObject {
	Q_OBJECT
public:
	int connect_test(RpcSocket *rpc);
private:
	QLocalSocket *m_socket;
};
