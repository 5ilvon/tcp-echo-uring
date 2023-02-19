#include "RingController.hpp"

RingController::RingController(int queueLength) {
	assert(io_uring_queue_init(4096, &ring, 0) >= 0);
}

// Submit prepared entries for submission queue
void RingController::submitEntries() {
	io_uring_submit(&ring);
}

// Wait CQE and return number of cqes
int RingController::waitComplition() {
	assert( io_uring_wait_cqe(&ring, &cqe) == 0);
	
	return io_uring_peek_batch_cqe(&ring, cqes, BACKLOG);
}

conn_info* RingController::getComplitionData() {
	return (conn_info *)io_uring_cqe_get_data(cqe);
}

// Add accept in SQ, fd - descriptor sock connection
void RingController::prepareAcceptEntry(int fd, conn_info *data)
{
	// Get pointer to first SQE
	io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	// Helper io_uring_prep_accept add in SQE operation ACCEPT.
	io_uring_prep_accept(sqe, fd, NULL, NULL, 0);

	// Set in user_data pointer to socketInfo client sock
	io_uring_sqe_set_data(sqe, data);
}

/**
 *  Add recv in SQ.
 */
void RingController::prepareReadEntry(int fd, size_t size, conn_info *data)
{
	// Get pointer to first SQE
	io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	// Helper io_uring_prep_recv add in SQE operation RECV, read from buf client sock
	io_uring_prep_recv(sqe, fd, &data->data, size, 0);

	// Set in user_data pointer to socketInfo client sock
	io_uring_sqe_set_data(sqe, data);
}

/**
 *  Add send in SQ buff.
 */
void RingController::prepareWriteEntry(int fd, size_t size, conn_info *data)
{
	// Get pointer to first SQE
	io_uring_sqe *sqe = io_uring_get_sqe(&ring);

	// Helper io_uring_prep_send add in SQE operation SEND, writing from buffer client sock.
	io_uring_prep_send(sqe, fd, &data->data, size, 0);

	// Set in user_data pointer to socketInfo client sock
	io_uring_sqe_set_data(sqe, data);
}

void RingController::prepareTimeoutEntry(int fd, conn_info *data) {
	time_t seconds = 5;
	static struct __kernel_timespec ts = {.tv_sec = seconds, .tv_nsec = 0};

	io_uring_sqe *sqe = io_uring_get_sqe(&ring);

	io_uring_prep_timeout(sqe, &ts, 0, 0);

	// Set in user_data pointer to socketInfo client sock
	io_uring_sqe_set_data(sqe, data);
}

int RingController::getSqeRes() {
	return cqe->res;
}

void RingController::seenCqe() {
	io_uring_cqe_seen(&ring, cqe);
}

RingController::~RingController (){
	io_uring_queue_exit(&ring); // Закрываем сокет
}