// udp_receiver.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/pool/singleton_pool.hpp>

#include <winsock2.h>
#include <mswsock.h>
#include <MSTcpIP.h>
#include <errno.h>

#include <ws2ipdef.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
	{
		printf("init socket failed\n");
		return -1;
	}

	//rtsp点播方式使用tcp连接，受系统tcp连接数2000限制
	unsigned short ulocal_start_port = 26000;
	unsigned short urecv_start_port = 16000;
	while (static int loop = 1)
	{
		SOCKET vt_tmp[1000];
		for (int i=1;i<=1000;++i)
		{
			SOCKET sclient= socket(PF_INET, SOCK_DGRAM, 0);
			if (sclient == INVALID_SOCKET)
			{
				printf("create socket() failed\n");
				return -1;
			}
			//int nbroadcast = 1;
			//setsockopt(sclient, SOL_SOCKET, SO_BROADCAST, (char *)&nbroadcast, sizeof(int));

			SOCKADDR_IN udpsvr;
			udpsvr.sin_family = AF_INET;
			udpsvr.sin_addr.s_addr = inet_addr("0.0.0.0");//htonl(ADDR_ANY)
			udpsvr.sin_port = htons(urecv_start_port+i*2);

			int err = bind(sclient, (SOCKADDR *)&udpsvr, sizeof SOCKADDR_IN);
			if (err != 0)
			{
				printf("bind() ip:%s port:%d failed\n",htonl(udpsvr.sin_addr.s_addr),ntohs(udpsvr.sin_port));
				continue;
			}
			vt_tmp[i-1] = sclient;
			//printf("open ip:%s port:%d success\n",htonl(udpsvr.sin_addr.s_addr),ntohs(udpsvr.sin_port));
		}
		for (int i=0;i<1000;i++)
		{
			closesocket(vt_tmp[i]);
		}
		Sleep(10);
		printf("loop:%d open 1000 ports success\n",loop++);
	}

	SOCKET sclient= socket(PF_INET, SOCK_DGRAM, 0);
	if (sclient == INVALID_SOCKET)
	{
		printf("create socket() failed\n");
		return -1;
	}
	//int nbroadcast = 1;
	//setsockopt(sclient, SOL_SOCKET, SO_BROADCAST, (char *)&nbroadcast, sizeof(int));

	SOCKADDR_IN udpsvr;
	udpsvr.sin_family = AF_INET;
	udpsvr.sin_addr.s_addr = inet_addr("0.0.0.0");//htonl(ADDR_ANY)
	udpsvr.sin_port = htons(7890);

	int err = bind(sclient, (SOCKADDR *)&udpsvr, sizeof SOCKADDR_IN);
	if (err != 0)
	{
		printf("bind() socket failed\n");
		return -1;
	}

	
	char buff[1024]={0};
	memset(buff, 0x0, 1024);

	//add multicast group 
	IP_MREQ mreq;
	mreq.imr_interface.s_addr = htonl(ADDR_ANY);
	mreq.imr_multiaddr.s_addr = inet_addr("225.0.0.37");
	setsockopt(sclient, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof mreq);
	
	while (1)
	{
		SOCKADDR_IN udpclient;
		int nlen = sizeof(SOCKADDR_IN);
		int ret = recvfrom(sclient, buff, 1024, 0, (SOCKADDR *)&udpclient, &nlen);
		if (ret < 0)
		{
			printf("recv error:%d\n", WSAGetLastError());
			break;
		}

		printf("sock:%d addr:%s:%d data:%s\n", sclient, inet_ntoa(udpclient.sin_addr), 
			ntohs(udpclient.sin_port),buff);
	}
	closesocket(sclient);
	WSACleanup();

	return 0;
}
