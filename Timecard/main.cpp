#include "myws.h"

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
	int err = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)yes, sizeof(yes));
	if (!err) {
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

		printf("accepted connection from %s, port %d\n",
			inet_ntoa(client.sin_addr), ntohs(client.sin_port));

		char status[] = "HTTP/1.0 200 OK";
		int n = send(sock, status, strlen(status), 0);
		if (n < 1) {
			printf("send : %d\n", WSAGetLastError());
			goto cleanup;
		}

		closesocket(sock);
	}

	WSACleanup();
	return 0;

cleanup:
	WSACleanup();
	return 1;
}