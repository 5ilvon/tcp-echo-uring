#include "Connection.hpp"
#include "RingController.hpp"

void Connection::handle(RingController *ringCtrl, std::unordered_map<int , Connection> &connections, int sock_listen_fd) {
	// identify operation by type
	if (type == ACCEPT) {
		// if have new messege -- add operation recv in SQ
		// read from client sock and continue listen serv sock
		Connection* conn = &connections[ringCtrl->getSqeRes()];
		conn->type = READ;
		conn->fd = ringCtrl->getSqeRes();
		ringCtrl->prepareReadEntry(conn->fd, MAX_MESSAGE_LEN, conn);
		ringCtrl->prepareAcceptEntry(sock_listen_fd, this);
	}
	else if (type == READ) {
		int bytes_read = ringCtrl->getSqeRes();
		// if readen 0 byte -- close sock
		// else -- send data to client
		if (bytes_read <= 0) {
			shutdown(fd, SHUT_RDWR);
		}
		else {
			type = TIMEOUT;
			ringCtrl->prepareTimeoutEntry(fd, this);
		}
	}
	else if (type == TIMEOUT) {
		type = WRITE;
		ringCtrl->prepareWriteEntry(fd, sizeof(data), this);
	}
	else if (type == WRITE) {
		type = READ;
		ringCtrl->prepareReadEntry(fd, MAX_MESSAGE_LEN, this);
	}
}