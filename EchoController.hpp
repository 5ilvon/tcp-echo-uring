#pragma once

#include <iostream>
#include <cstring>
#include <cassert>
#include <ctime>


#include <liburing.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAX_CONNECTIONS 4096
#define BACKLOG 512

class EchoController
{
	public:
		EchoController() = default;
		virtual ~EchoController(); // Реализовать закрытие сокета и ринга

		void init(int);
		void run();

	private:
		int sock_listen_fd;
};