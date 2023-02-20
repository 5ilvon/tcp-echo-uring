#pragma once

#include <iostream>
#include <cstring>
#include <cassert>
#include <ctime>

#include <liburing.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "Connection.hpp"

#define MAX_CONNECTIONS 4096
#define BACKLOG 512

class RingController
{
	public:
		RingController(int);
		RingController() = default;
		virtual ~RingController();

	public:
		void submitEntries();
		int waitComplition();
		Connection* getComplitionData();
		int getSqeRes();

		void seenCqe();

		void prepareAcceptEntry(int, Connection *);
		void prepareTimeoutEntry(int, Connection *);
		void prepareReadEntry(int fd, size_t size, Connection *);
		void prepareWriteEntry(int fd, size_t size, Connection *);

	private:
		//struct client_info{int desc; int socket; char data;} client_info;
		io_uring ring;
		io_uring_cqe *cqe;
		io_uring_cqe *cqes[BACKLOG];
};