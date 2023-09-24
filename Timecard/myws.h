#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

namespace my {
	int tcp_socket_listen_port(int port, int backlog, char* error);
	int tcp_scoket(char* error);
	bool tcp_reuse(int sockfd_listen, char* error);
	bool tcp_bind(int sockfd_listen, int port, char* error);
	bool tcp_listen(int sockfd_listen, int backlog, char* error);
	int tcp_accept(int sockfd_listen, char* error, bool is_verbose = true);
}