/*
#include "myws.h"

int main() {
	const int BACKLOG = 5;
	const int READ_TIMEOUT = 10;
	const size_t ERR_SIZE = 512;
	char error[ERR_SIZE];
	int sockfd_listen;
	int sockfd_client;


}
*/

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "ws2_32.lib")
//#pragma comment (lib, "mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "12345"

int main()
{
	int ret = 0;

	// WINSOCKライブラリ初期化
	WSADATA wsadata;
	ret = WSAStartup(WINSOCK_VERSION, &wsadata);
	if (ret != 0)
	{
		printf("WSAStartup failed with error: %d\n", ret);
		return 1;
	}

	// ソケット情報
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// アドレス情報取得
	struct addrinfo* result = NULL;
	ret = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (ret != 0)
	{
		printf("getaddrinfo failed with error: %d\n", ret);
		WSACleanup();
		return 1;
	}

	// ソケット作成
	SOCKET listenSocket = INVALID_SOCKET;
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// ソケット割当
	ret = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (ret == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// アドレス情報解放
	freeaddrinfo(result);

	// 接続受付開始
	ret = listen(listenSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// クライアント情報
	struct sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);

	// 接続受付
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// クライアント情報
	char str_buffer[max(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];
	inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, str_buffer, sizeof(str_buffer));
	printf("accepted connection from %s, port=%d\n",
		str_buffer,
		ntohs(clientAddr.sin_port)
	);

	// 接続受付ソケット解放
	closesocket(listenSocket);

	// 受信
	for (;;)
	{
		// 受信
		char recvbuf[DEFAULT_BUFLEN];
		ret = recv(clientSocket, recvbuf, sizeof(recvbuf), 0);
		if (ret > 0)
		{
			printf("Bytes received: %d\n", ret);

			// 送信
			ret = send(clientSocket, recvbuf, ret, 0);
			if (ret == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(clientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", ret);
		}
		else if (ret == 0)
		{
			printf("Connection closing...\n");
			break;
		}
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
	}

	// 切断要求
	ret = shutdown(clientSocket, SD_SEND);
	if (ret == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	// ソケット解放
	closesocket(clientSocket);

	// WINSOCKライブラリ解放
	WSACleanup();

	return 0;
}