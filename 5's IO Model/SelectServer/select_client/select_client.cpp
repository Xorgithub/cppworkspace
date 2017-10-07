// select_client.cpp : �������̨Ӧ�ó������ڵ㡣
//
/************************************************************************/
/* select�ĺ���ԭ�ͣ�
   int select
			(
			int nfds, //Winsock�д˲���������
			fd_set* readfds, //���пɶ�����Socket
			fd_set* writefds, //���п�д����Socket
			fd_set* exceptfds, //�����쳣����Socket
			const struct timeval* timeout //������ģʽ���������ȴ�ʱ��
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

	//selectģ�ʹ������
	fd_set  fdSocket;
	FD_ZERO(&fdSocket);

	SOCKET maxfd = 0;
	for (int i=0;i<FD_SETSIZE-4;i++)// �����������׽���Ҳ��ռ��һ��
	{
		SOCKET connfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(connect(connfd, (sockaddr*)&server, sizeof(server)) < 0)
		{
			printf("connect error:%d\n", WSAGetLastError());
			break;
		}
		
		if (connfd > maxfd)
			maxfd = connfd; /* ����maxfdֵ*/

		//1.��ʼ���׽��ּ��ϣ���Ӽ���socket���������
		FD_SET(connfd, &fdSocket);
	}

	timeval tv={10, 0};//���û�ж�д�¼�������select������ʱʱ��󷵻�
	while (1)
	{
		Sleep(3*1000);
		//2. �����ϵ�һ���������ݸ�select����
		//�����¼�����ʱ��select�Ƴ�δ����socket,Ȼ�󷵻�   ����˵select����ʱ ���ϵ���socket���Ƿ����¼���socket
		fd_set fdRead = fdSocket;
		fd_set fdWrite = fdSocket;
		fd_set fdExcept = fdSocket;
		int nRet = ::select(maxfd+1, &fdRead, &fdWrite, &fdExcept, &tv);
		if (nRet > 0)
		{
			//ͨ���Ƚ�ԭ����fdSocket�����뾭�������fdRead����
			for (UINT i=0;i<fdSocket.fd_count;i++)
			{
				if (FD_ISSET(fdSocket.fd_array[i], &fdRead)) //�пɶ���
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
				
				if (FD_ISSET(fdSocket.fd_array[i], &fdWrite)) //�п�д��
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

				if (FD_ISSET(fdSocket.fd_array[i], &fdExcept)) //���쳣��
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