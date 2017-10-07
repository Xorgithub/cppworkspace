// SelectServer.cpp : Defines the entry point for the console application.
//

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

	//CInitSock initSock;

	USHORT nPort = 4567; //监听的端口
	SOCKET sListen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sListen == INVALID_SOCKET)
	{
		printf("socket error\n");
		return -1;
	}

	sockaddr_in servAddr = {0};
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = ::htons(nPort);
	servAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (SOCKET_ERROR == ::bind(sListen, (sockaddr*)&servAddr, sizeof(servAddr)))
	{
		int nError = ::GetLastError();
		printf("bind error:%d\n", nError);
		return -1;
	}

	::listen(sListen, 5);
	//select模型处理过程

	//1.初始化套接字集合，添加监听socket到这个集合
	timeval tv= {3, 0};

	fd_set  fdSocket;
	FD_ZERO(&fdSocket);
	FD_SET(sListen, &fdSocket);
	while (TRUE)
	{
		//2. 将集合的一个拷贝传递给select函数
		//当有事件发生时，select移除未决的socket,然后返回   就是说select返回时 集合的中socket就是发生事件的socket
		fd_set fdRead = fdSocket;
		int nRet = ::select(0, &fdRead, NULL, NULL, &tv);
		if (nRet > 0)
		{
			//通过比较原来的fdSocket集合与经过处理的fdRead集合
			for (UINT i=0;i<fdSocket.fd_count;i++)
			{
				if (FD_ISSET(fdSocket.fd_array[i], &fdRead))
				{
					if (fdSocket.fd_array[i] == sListen) //监听套接字接收到新连接  为神马分两种情况：因为触发Read集合的情况有两种：有连接来了或数据可读了….
					{
						//
						if (fdSocket.fd_count < FD_SETSIZE) //FD_SETSIZE=64
						{
							sockaddr_in addrRemote = {0};
							int nAddrLen = sizeof(addrRemote);
							SOCKET sNew = ::accept(sListen, (sockaddr*)&addrRemote, &nAddrLen);
							FD_SET(sNew, &fdSocket);
						}
						else
						{
							printf("too many connections\n");
							continue;
						}
					}
					else  //有可读的
					{
						char buff[256]={0};
						int nRecv = ::recv(fdSocket.fd_array[i], buff, sizeof(buff), 0);
						if (nRecv > 0)
						{
							buff[nRecv] = '\0';
							printf("socket:%d recv data:%s\n", fdSocket.fd_array[i], buff);
							send(fdSocket.fd_array[i], buff, nRecv, 0);
						}
						else
						{
							::closesocket(fdSocket.fd_array[i]);
							FD_CLR(fdSocket.fd_array[i], &fdSocket);
						}
					}
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

	::closesocket(sListen); //与socket配对写
	printf("*******************************");

	getchar();

	return 0;

}