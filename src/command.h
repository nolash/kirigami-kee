#ifndef _KEE_CMD_H
#define _KEE_CMD_H

int process_rpc_command(Backend *backend, char *buf, size_t buf_len, char *result);
int preview_command(char *in, size_t in_len, char *out, char *result);

#endif // _KEE_CMD_H
