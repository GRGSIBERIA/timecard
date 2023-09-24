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

	// WINSOCK���C�u����������
	WSADATA wsadata;
	ret = WSAStartup(WINSOCK_VERSION, &wsadata);
	if (ret != 0)
	{
		printf("WSAStartup failed with error: %d\n", ret);
		return 1;
	}

	// �\�P�b�g���
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// �A�h���X���擾
	struct addrinfo* result = NULL;
	ret = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (ret != 0)
	{
		printf("getaddrinfo failed with error: %d\n", ret);
		WSACleanup();
		return 1;
	}

	// �\�P�b�g�쐬
	SOCKET listenSocket = INVALID_SOCKET;
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// �\�P�b�g����
	ret = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (ret == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// �A�h���X�����
	freeaddrinfo(result);

	// �ڑ���t�J�n
	ret = listen(listenSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// �N���C�A���g���
	struct sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);

	// �ڑ���t
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// �N���C�A���g���
	char str_buffer[max(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];
	inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, str_buffer, sizeof(str_buffer));
	printf("accepted connection from %s, port=%d\n",
		str_buffer,
		ntohs(clientAddr.sin_port)
	);

	// �ڑ���t�\�P�b�g���
	closesocket(listenSocket);

	// ��M
	for (;;)
	{
		// ��M
		char recvbuf[DEFAULT_BUFLEN];
		ret = recv(clientSocket, recvbuf, sizeof(recvbuf), 0);
		if (ret > 0)
		{
			printf("Bytes received: %d\n", ret);

			// ���M
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

	// �ؒf�v��
	ret = shutdown(clientSocket, SD_SEND);
	if (ret == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	// �\�P�b�g���
	closesocket(clientSocket);

	// WINSOCK���C�u�������
	WSACleanup();

	return 0;
}