#ifndef _RPC_H
#define _RPC_H

#include <QLocalServer>
#include "backend.h"

#ifndef RPC_BUFFER_SIZE
#define RPC_BUFFER_SIZE	4096
#endif

#ifndef RPC_COMMAND_SIZE
#define RPC_COMMAND_SIZE 1048576
#endif


/**
 *
 * \brief Local socket enabling RPC commands for GUI operations.
 */
class RpcSocket : public QLocalServer {
	Q_OBJECT

public:
	RpcSocket(Backend *b);
protected:
	void incomingConnection(quintptr fd) override;
private:
	Backend *m_backend;
};

#endif // _RPC_H
