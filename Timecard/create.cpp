#include "myws.h"

const bool my::createwsa(WSADATA& wsa) {
	int err = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (err != 0) {
		switch (err) {
		case WSASYSNOTREADY:
			printf("WSASYSNOTREADY\n");
			return false;
		case WSAVERNOTSUPPORTED:
			printf("WSAVERNOTSUPPORTED\n");
			return false;
		case WSAEINPROGRESS:
			printf("WSAEINPROGRESS\n");
			return false;
		case WSAEPROCLIM:
			printf("WSAEPROCLIM\n");
			return false;
		case WSAEFAULT:
			printf("WSAEFAULT\n");
			return false;
		}
	}
	return true;
}

bool my::createsock(SOCKET& sock) {
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		return false;
	}
	return true;
}