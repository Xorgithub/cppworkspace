// TestClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <stdlib.h>
#include <iostream>
#include <process.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

#include <Mswsock.h>

#define SERVER_PORT 4567
#define SERVER_IP "192.168.1.8"
#define PPPOE_IP "100.64.130.47"

#include <vector>
int _tmain(int argc, _TCHAR* argv[])
{
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout<<"init sockt failed"<<endl;
		return -1;
	}

	SOCKADDR_IN svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(SERVER_PORT);
#ifdef _DEBUG
	svraddr.sin_addr.s_addr = inet_addr(SERVER_IP);
#else
	svraddr.sin_addr.s_addr = inet_addr(PPPOE_IP);
#endif

	int iconncount = 0;
	SOCKET sClient = INVALID_SOCKET;

	SOCKADDR_IN local;
	int ilen = sizeof local;

	UINT uMinPort = 65535;
	UINT uMaxPort = 1024;
	while (1)
	{
		sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sClient == INVALID_SOCKET)
		{
			cout<<"invalid socket"<<endl;
			return -1;
		}

		//非阻塞模式
		//u_long iMode = 1;
		//ioctlsocket(sServer, FIONBIO, &iMode);

		//设置为socket重用,防止服务器崩溃后端口能够尽快再次使用或共其他的进程使用
		int nOpt = 1;
		int errorCode = setsockopt(sClient, SOL_SOCKET, SO_REUSEADDR, (char*)&nOpt, sizeof(nOpt));
		if(SOCKET_ERROR == errorCode)
		{
			
			cout<<"socket SO_REUSEADDR mode error"<<endl;
			return -1;
		}
		int ret = connect(sClient, (SOCKADDR *)&svraddr, sizeof(SOCKADDR));
		if(SOCKET_ERROR == ret)
		{
			int dwCode = WSAGetLastError();
			cout<<"connect failed:"<<dwCode<<endl;
			cout<<"min port:"<<uMinPort<<"max port:"<<uMaxPort<<endl;
			getchar();
			continue;
		}

		iconncount++; // 输出io速度限制连接速度，大于3w在输出连接数

		//tcp连接四元组 {源IP地址、源端口、目的IP地址、目的端口}
		struct sockaddr_in local, remote;
		char local_ip[20];
		char remote_ip[20];
		int local_len = sizeof(local);
		int remote_len = sizeof(remote);

		getsockname(sClient, (struct sockaddr *)&local, &local_len);
		getpeername(sClient, (struct sockaddr *)&remote, &remote_len);

		strcpy(local_ip,inet_ntoa(local.sin_addr));
		strcpy(remote_ip,inet_ntoa(remote.sin_addr));

		if (ntohs(local.sin_port) < uMinPort)
		{
			uMinPort = ntohs(local.sin_port);
		}
		else if (ntohs(local.sin_port) >= uMaxPort)
		{
			uMaxPort = ntohs(local.sin_port);
		}
		
		printf("%d socket:%d local %s:%d remote %s:%d\n", iconncount, sClient, 
			local_ip, ntohs(local.sin_port), remote_ip, ntohs(remote.sin_port));
		
		ret = send(sClient, "hello", sizeof "hello", 0);

		char buff[8]={0x0};
		ret = recv(sClient, buff, 8, 0);
		buff[ret]='\0';
		cout<<"recv data:"<<buff<<endl;
		//关闭系统的接收与发送缓冲区
		int nBufferSize = 0;
		setsockopt(sClient,SOL_SOCKET,SO_SNDBUF,(char*)&nBufferSize,sizeof(int));
		setsockopt(sClient,SOL_SOCKET,SO_RCVBUF,(char*)&nBufferSize,sizeof(int));
	}

	getchar();

	return 0;
}

