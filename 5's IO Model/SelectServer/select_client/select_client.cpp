// select_client.cpp : 定义控制台应用程序的入口点。
//
/************************************************************************/
/* select的函数原型：
   int select
			(
			int nfds, //Winsock中此参数无意义
			fd_set* readfds, //进行可读检测的Socket
			fd_set* writefds, //进行可写检测的Socket
			fd_set* exceptfds, //进行异常检测的Socket
			const struct timeval* timeout //非阻塞模式中设置最大等待时间
			)                                                           */
/************************************************************************/
#include "stdafx.h"
#include <ctime>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2,0),&wsaData) != 0)
	{
		printf("WASStarup failed\n");
		return -1;
	}

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port   = htons(4567);
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	//select模型处理过程
	fd_set  fdSocket;
	FD_ZERO(&fdSocket);

	SOCKET maxfd = 0;
	for (int i=0;i<FD_SETSIZE-4;i++)// 服务器监听套接字也会占用一个
	{
		SOCKET connfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(connect(connfd, (sockaddr*)&server, sizeof(server)) < 0)
		{
			printf("connect error:%d\n", WSAGetLastError());
			break;
		}
		
		if (connfd > maxfd)
			maxfd = connfd; /* 设置maxfd值*/

		//1.初始化套接字集合，添加监听socket到这个集合
		FD_SET(connfd, &fdSocket);
	}

	timeval tv={10, 0};//如果没有读写事件发生，select经过超时时间后返回
	while (1)
	{
		Sleep(3*1000);
		//2. 将集合的一个拷贝传递给select函数
		//当有事件发生时，select移除未决的socket,然后返回   就是说select返回时 集合的中socket就是发生事件的socket
		fd_set fdRead = fdSocket;
		fd_set fdWrite = fdSocket;
		fd_set fdExcept = fdSocket;
		int nRet = ::select(maxfd+1, &fdRead, &fdWrite, &fdExcept, &tv);
		if (nRet > 0)
		{
			//通过比较原来的fdSocket集合与经过处理的fdRead集合
			for (UINT i=0;i<fdSocket.fd_count;i++)
			{
				if (FD_ISSET(fdSocket.fd_array[i], &fdRead)) //有可读的
				{
					char buff[256]={0};
					int nRecv = ::recv(fdSocket.fd_array[i], buff, sizeof(buff), 0);
					if (nRecv > 0)
					{
						buff[nRecv] = '\0';
						printf("socket:%d recv data:%s\n", fdSocket.fd_array[i], buff);
					}
					else
					{
						::closesocket(fdSocket.fd_array[i]);
						FD_CLR(fdSocket.fd_array[i], &fdSocket);
						printf("recv error:%d\n", WSAGetLastError());
					}
				}
				
				if (FD_ISSET(fdSocket.fd_array[i], &fdWrite)) //有可写的
				{
					char buff[256]={0};
					memcpy(buff, "hello world", sizeof "hello world");
					int ret = ::send(fdSocket.fd_array[i], buff, sizeof("hello world"), 0);
					if (ret < 0)
					{
						::closesocket(fdSocket.fd_array[i]);
						FD_CLR(fdSocket.fd_array[i], &fdSocket);
						printf("send error:%d\n", WSAGetLastError());
					}
				}

				if (FD_ISSET(fdSocket.fd_array[i], &fdExcept)) //有异常的
				{
					
					printf("select except fd:%d\n", WSAGetLastError());
					::closesocket(fdSocket.fd_array[i]);
					FD_CLR(fdSocket.fd_array[i], &fdSocket);
				}
			}
		}
		else if (nRet == 0)
		{
			printf("time:%ld select timeout.\n", time(0));
			continue;
		}
		else
		{
			printf("nRet litter 0, error\n");
			break;
		}
	}

	printf("*******************************");

	getchar();

	return 0;
}