cmake_minimum_required(VERSION 3.22)

project(echo_serv
		VERSION 0.1
		DESCRIPTION "Test"
)

add_executable(
	echo echo-serv.cpp 
	EchoController.cpp 
	RingController.cpp )
target_link_libraries(echo uring)
