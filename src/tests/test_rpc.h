#include "rpc.h"


/// Rpc socket implementation for use in tests.
class RpcTest : public QObject {
	Q_OBJECT
public:
	int connect_test(RpcSocket *rpc);
private:
	QLocalSocket *m_socket;
};
