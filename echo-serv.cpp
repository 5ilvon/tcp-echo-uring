#include "EchoController.hpp"

int main(int argc, char *argv[])
{
	char *p;
	EchoController test_server;
	if(argc > 1) test_server.init( strtol(argv[1], &p, 10) );
	else test_server.init( 1337 );

	test_server.run();

	return 0;
}