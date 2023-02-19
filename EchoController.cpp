#include "EchoController.hpp"
#include "RingController.hpp"

static std::unordered_map<int , conn_info> conns;

void EchoController::init(int portNumber) {
	sockaddr_in serv_addr;
	sock_listen_fd = socket(AF_INET, SOCK_STREAM, 0); // Создаем дескриптор сокета
	setsockopt(sock_listen_fd, SOL_SOCKET, SO_REUSEADDR, (const void*)1, sizeof((const void*)1));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portNumber); //convert to network bytes order
	serv_addr.sin_addr.s_addr = INADDR_ANY; // Слушаем все входящие соединения

	assert(bind(sock_listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0); // Биндим дескриптор сокета с параметрами serv_addr
	assert(listen(sock_listen_fd, BACKLOG) >= 0); // Подключаем сокет
}

void EchoController::run() {
	conns[sock_listen_fd] = {sock_listen_fd, ACCEPT, };

	RingController ringCtrl(MAX_CONNECTIONS);
	ringCtrl.prepareAcceptEntry(sock_listen_fd, &(conns[sock_listen_fd])); // Готовим задачу ядру на прослушивание открытого сокета

	while (1) {
		ringCtrl.submitEntries(); // Отправляем все entries на исполнение

		int numEntries = ringCtrl.waitComplition(); // Ждем CQEs и узнаем количество завершенных

		for (int i = 0; i < numEntries; i++) {
			conn_info *user_data = ringCtrl.getComplitionData(); // data - адрес элемента в client_info, в нем информация с сокетом

			// identify operation by type
			if (user_data->type == ACCEPT) {
				int sock_conn_fd = ringCtrl.getSqeRes();
				// if have new messege -- add operation recv in SQ
				// read from client sock and continue listen serv sock
				conns[sock_conn_fd].type = READ;
				conns[sock_conn_fd].fd = sock_conn_fd;
				ringCtrl.prepareReadEntry(sock_conn_fd, MAX_MESSAGE_LEN, &conns[sock_conn_fd]);
				ringCtrl.prepareAcceptEntry(sock_listen_fd, &conns[sock_listen_fd]);
			}
			else if (user_data->type == READ) {
				int bytes_read = ringCtrl.getSqeRes();
				// if readen 0 byte -- close sock
				// else -- send data to client
				if (bytes_read <= 0) {
					shutdown(user_data->fd, SHUT_RDWR);
				}
				else {
					conns[user_data->fd].type = TIMEOUT;
					ringCtrl.prepareTimeoutEntry(user_data->fd, &conns[user_data->fd]);
				}
			}
			else if (user_data->type == TIMEOUT) {
				conns[user_data->fd].type = WRITE;
				ringCtrl.prepareWriteEntry(user_data->fd, sizeof(user_data->data), &conns[user_data->fd]);
			}
			else if (user_data->type == WRITE) {
				conns[user_data->fd].type = READ;
				ringCtrl.prepareReadEntry(user_data->fd, MAX_MESSAGE_LEN, &conns[user_data->fd]);
			}

			ringCtrl.seenCqe();
		}
	}
}

EchoController::~EchoController () {
	assert(close(sock_listen_fd)); // Закрываем сокет
}