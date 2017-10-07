// getpeername.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <winsock.h>
#include <stdio.h>

#pragma comment(lib, "wsock32")
using namespace std;

int main() 
{
	WSADATA wsadata;
	WORD version = MAKEWORD(2,0);
	if (WSAStartup(version,&wsadata) != 0)
	{
		cout<<"init socket failed"<<endl;
		return -1;
	}
	
	SOCKET sockServer = socket(AF_INET,SOCK_STREAM,0);

	//设置为socket重用,防止服务器崩溃后端口能够尽快再次使用或共其他的进程使用
	int nOpt = 1;
	int errorCode = setsockopt(sockServer, SOL_SOCKET, SO_REUSEADDR, (char*)&nOpt, sizeof(nOpt));
	if(SOCKET_ERROR == errorCode)
	{

		cout<<"socket SO_REUSEADDR mode error"<<endl;
		return -1;
	}

	SOCKADDR_IN svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.S_un.S_addr = INADDR_ANY;
	svraddr.sin_port = htons(4567);
	
	if (bind(sockServer,(LPSOCKADDR)&svraddr,sizeof(svraddr)) == SOCKET_ERROR)
	{
		cout<<"bind failed:"<<WSAGetLastError()<<endl;
		return -1;
	}
	
	if (listen(sockServer,10) == SOCKET_ERROR)
	{
		cout<<"listen failed:"<<WSAGetLastError()<<endl;
		return -1;
	}
	
	cout<<"server ip:"<<inet_ntoa(svraddr.sin_addr)<<endl;
	cout<<"server port:"<<ntohs(svraddr.sin_port)<<endl;
	
	int iconncount = 0;
	while (1)
	{
		// 注意：异步acceptex使用相应GetAcceptExSockaddrs函数
		SOCKET sockClient = accept(sockServer,NULL,NULL);
		if (sockClient == INVALID_SOCKET)
		{
			cout<<"accept failed:"<<WSAGetLastError()<<endl;
			break;
		}

		//设置为socket重用,防止服务器崩溃后端口能够尽快再次使用或共其他的进程使用
		int nOpt = 1;
		int errorCode = setsockopt(sockClient, SOL_SOCKET, SO_REUSEADDR, (char*)&nOpt, sizeof(nOpt));
		if(SOCKET_ERROR == errorCode)
		{

			cout<<"socket SO_REUSEADDR mode error"<<endl;
			return -1;
		}

		iconncount++;

		SOCKADDR_IN local;
		int nlocal = sizeof(local);
		memset((void *)&local,0,sizeof(local));

		SOCKADDR_IN remote;
		int nremote = sizeof(remote);
		memset((void *)&remote,0,sizeof(remote));

		getsockname(sockClient, (SOCKADDR *)&local, &nlocal);
		getpeername(sockClient,(SOCKADDR *)&remote, &nremote);

		char local_ip[20]={0x0};
		char remote_ip[20]={0x0};

		strcpy(local_ip, inet_ntoa(local.sin_addr));
		strcpy(remote_ip,inet_ntoa(remote.sin_addr));

		printf("%d socket:%d local %s:%d remote %s:%d\n", iconncount, sockClient, 
			local_ip, ntohs(local.sin_port), remote_ip, ntohs(remote.sin_port));

		char buff[8]={0x0};
		int ret = recv(sockClient, buff, 8, 0);
		buff[ret]='\0';
		cout<<"recv data:"<<buff<<endl;

		ret = send(sockClient, buff, ret, 0);
		//关闭系统的接收与发送缓冲区
		int nBufferSize = 0;
		setsockopt(sockClient,SOL_SOCKET,SO_SNDBUF,(char*)&nBufferSize,sizeof(int));
		setsockopt(sockClient,SOL_SOCKET,SO_RCVBUF,(char*)&nBufferSize,sizeof(int));
	}
	
	WSACleanup();

	getchar();
}