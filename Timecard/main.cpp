#include "myws.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

int main() {
	WSADATA wsa;
	SOCKET sock;
	sockaddr_in addr;
	sockaddr_in client;

	if (!my::createwsa(wsa)) {
		WSACleanup();
		return 1;
	}

	if (!my::createsock(sock)) {
		WSACleanup();
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(8123);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	BOOL yes = 1;
	int err = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));
	if (err != 0) {
		printf("setsockopt : %d\n", WSAGetLastError());
		goto cleanup;
	}

	if (bind(sock, (sockaddr*)&addr, sizeof(addr)) != 0) {
		printf("bind : %d\n", WSAGetLastError());
		goto cleanup;
	}
	if (listen(sock, 5) != 0) {
		printf("listen : %d\n", WSAGetLastError());
		goto cleanup;
	}

	while(1) {
		int len = sizeof(client);
		sock = accept(sock, (sockaddr*)&client, &len);
		if (sock == INVALID_SOCKET) {
			printf("accept : %d\n", WSAGetLastError());
			goto cleanup;
		}

		char buf[256];
		inet_ntop(client.sin_family, &client.sin_addr, buf, 256);
		printf("accepted connection from %s, port %d\n",
			buf, ntohs(client.sin_port));

		char status[] = "HTTP/1.0 200 OK";
		int n = send(sock, status, (int)strlen(status), 0);
		if (n < 1) {
			printf("send : %d\n", WSAGetLastError());
			goto cleanup;
		}

		closesocket(sock);
	}

	WSACleanup();
	return 0;

cleanup:
	closesocket(sock);
	WSACleanup();
	return 1;
}