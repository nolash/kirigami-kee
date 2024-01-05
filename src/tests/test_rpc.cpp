#include "backend.h"
#include "rpc.h"


int process_rpc_command(Backend *backend, char *buf, size_t buf_len, char *result);

int main() {
	int r;
	char r_rpc;
	Backend backend;
	char data[1024];
	int c;
	CreditListModel credit_model;

	backend.set_credit_list(&credit_model);

	strcpy(data, "aeJxjScvPT2JOSiwCAA1QAuM=");
	c = strlen(data);
	data[0] = 0x1;

	r = process_rpc_command(&backend, data, c+1, &r_rpc);

	return r;
}
