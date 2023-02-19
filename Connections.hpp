#ifndef CONNS
#define CONNS
#include <unordered_map>
#define MAX_MESSAGE_LEN 2048
struct conn_info{int fd; unsigned int type; char data[MAX_MESSAGE_LEN];};
#endif