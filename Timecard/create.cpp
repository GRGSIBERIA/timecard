#include "myws.h"

int my::tcp_socket_listen_port(int port, int backlog, char* error) {
	int sockfd = my::tcp_scoket(error);

	if (sockfd < 0) return -1;

	if (!my::tcp_reuse(sockfd, error)) return -1;
	if (!my::tcp_bind(sockfd, port, error)) return -1;
	if (!my::tcp_listen(sockfd, backlog, error)) return -1;

	return sockfd;
}

int my::tcp_scoket(char* error) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd <= 0) {
		int save = errno;
		strcpy(error, strerror(save));
		return -1;
	}
	return fd;
}

bool my::tcp_reuse(int sockfd_listen, char* error) {
	const int reuse_yes = 1;
	int ret = setsockopt(
		sockfd_listen, SOL_SOCKET, SO_REUSEADDR,
		(const char*)&reuse_yes, sizeof(reuse_yes));

	if (ret != 0) {
		int save = errno;
		strcpy(error, strerror(save));
		return false;
	}
	return true;
}

bool my::tcp_bind(int sockfd_listen, int port, char* error) {
	sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	int ret = bind(sockfd_listen, (sockaddr*)&addr, sizeof(addr));
	if (ret != 0) {
		int save = errno;
		strcpy(error, strerror(save));
		return false;
	}
	return true;
}

bool my::tcp_listen(int sockfd_listen, int backlog, char* error) {
	int ret = listen(sockfd_listen, backlog);
	if (ret != 0) {
		int save = errno;
		strcpy(error, strerror(save));
		return false;
	}
	return true;
}

int my::tcp_accept(int sockfd_listen, char* error, bool is_verbose = true) {
	sockaddr_in client;
	socklen_t size_client = sizeof(client);

	int fd = accept(sockfd_listen, (sockaddr*)&client, &size_client);

	if (fd <= 0) {
		int save = errno;
		strcpy(error, strerror(save));
		return -1;
	}

	if (is_verbose) {
		printf("connected from: %s, port %d\n",
			inet_ntoa(client.sin_addr), ntohs(client.sin_port));
	}
	return fd;
}