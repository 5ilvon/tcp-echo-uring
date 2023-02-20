#include "EchoController.hpp"
#include "RingController.hpp"

static std::unordered_map<int , Connection> conns;

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
	Connection* listen_conn = new Connection();
	conns[sock_listen_fd] = *listen_conn;

	RingController ringCtrl(MAX_CONNECTIONS);
	ringCtrl.prepareAcceptEntry(sock_listen_fd, &(conns[sock_listen_fd])); // Готовим задачу ядру на прослушивание открытого сокета

	while (1) {
		ringCtrl.submitEntries(); // Отправляем все entries на исполнение

		int numEntries = ringCtrl.waitComplition(); // Ждем CQEs и узнаем количество завершенных

		for (int i = 0; i < numEntries; i++) {
			Connection *user_data = ringCtrl.getComplitionData();

			user_data->handle(&ringCtrl, conns, sock_listen_fd);

			ringCtrl.seenCqe();
		}
	}
}

EchoController::~EchoController () {
	assert(close(sock_listen_fd)); // Закрываем сокет
}