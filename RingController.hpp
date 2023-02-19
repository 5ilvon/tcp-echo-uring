#ifndef RINGCONTROLLER_CLASS
#define RINGCONTROLLER_CLASS

#include <iostream>
#include <cstring>
#include <cassert>
#include <ctime>

#include <liburing.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "Connections.hpp"

#define MAX_CONNECTIONS 4096
#define BACKLOG 512
#define MAX_MESSAGE_LEN 2048

class RingController
{
	public:
		RingController(int);
		RingController() = default;
		virtual ~RingController();

	public:
		void submitEntries();
		int waitComplition();
		conn_info* getComplitionData();
		int getSqeRes();

		void seenCqe();

		void prepareAcceptEntry(int, conn_info *);
		void prepareTimeoutEntry(int, conn_info *);
		void prepareReadEntry(int fd, size_t size, conn_info *);
		void prepareWriteEntry(int fd, size_t size, conn_info *);

	private:
		//struct client_info{int desc; int socket; char data;} client_info;
		io_uring ring;
		io_uring_cqe *cqe;
		io_uring_cqe *cqes[BACKLOG];
};

#endif