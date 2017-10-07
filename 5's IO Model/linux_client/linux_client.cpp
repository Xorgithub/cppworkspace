// linux_client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#pragma comment(lib,"WS2_32.lib")

#define MAXLINE 1024
int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	struct sockaddr_in servaddr;

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(6888);
	//inet_pton(AF_INET, servInetAddr, &servaddr.sin_addr);
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.128");
	memset(&(servaddr.sin_zero), 0x0, 8);

	while (1)
	{
		SOCKET servfd = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(servfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) 
		{
			printf("connect error:%d\n", WSAGetLastError());
			closesocket(servfd);
			Sleep(3000);
			continue;
		}

		int ret = send(servfd, "hello redhat", strlen("hello redhat"), 0);
		if (ret <= 0)
		{
			printf("send error:%d\n", WSAGetLastError());
			closesocket(servfd);
			Sleep(3000);
			continue;
		}
		printf("connect server success\n");

		char buf[MAXLINE];
		while (1)
		{
			int ret = recv(servfd, buf, MAXLINE, 0);
			if (ret <= 0)
			{
				printf("recv error:%d\n", WSAGetLastError());
				closesocket(servfd);
				Sleep(3000);
				break;
			}
			buf[ret]='\0';
			printf("server:%s\n", buf);
		}
	}
	
	WSACleanup();

	return 0;
}

