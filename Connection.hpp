#pragma once

#include <unordered_map>

class RingController;

#define MAX_MESSAGE_LEN 2048

enum
{
	ACCEPT,
	READ,
	WRITE,
	TIMEOUT
};

class Connection
{
	public:
	void handle(RingController *, std::unordered_map<int , Connection> &, int );
	char data[MAX_MESSAGE_LEN];
	int fd = 3;
	
	private:
	unsigned int type = ACCEPT;
};