// udp_sender.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <winsock2.h>
#include <mswsock.h>
#include <MSTcpIP.h>
#include <errno.h>

#include <ws2ipdef.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#define MULTI_IP "239.0.0.37"

#define RECV_START_PORT 16000

#define	LOCAL_BIND_PORT 20260

#define MAX_FD 16
fd_set fdwrite;
SOCKET fd_arr[MAX_FD] = {0};
const char *strhelloworld = "hello world";

unsigned int __stdcall threadfunc(void *)
{
	while (1)
	{
		fd_set fd_tmp = fdwrite;
		timeval tmv;
		tmv.tv_sec = 1;
		tmv.tv_usec = 0;
		int ret = select(0, NULL, &fd_tmp, NULL, &tmv);
		if (ret < 0)
		{
			printf("select failed:%d\n",ret);
			break;
		}
		else
		{
			for (unsigned int i=0;i<fd_tmp.fd_count;i++)
			{
				SOCKET fd = fd_tmp.fd_array[i];
				if (fd > 0)
				{
					unsigned short svrport = RECV_START_PORT+2*i;
					SOCKADDR_IN udpsvr;
					udpsvr.sin_family = AF_INET;
					udpsvr.sin_addr.s_addr = inet_addr(MULTI_IP);
					udpsvr.sin_port = htons(svrport);

					char buff[1024];
					sprintf(buff, "%s %d", strhelloworld, svrport);
					int len = strlen(buff)+1;
					int nsend = sendto(fd,buff,len,0,(SOCKADDR *)&udpsvr,sizeof(SOCKADDR_IN));
					if (nsend < 0)
					{
						printf("sendto error:%d\n", WSAGetLastError());
						continue;
					}

					printf("sock:%d dst addr:%s:%d send data:%s\n", 
						fd, inet_ntoa(udpsvr.sin_addr), ntohs(udpsvr.sin_port),buff);
				}
			}
			/*for (int i=0;i<MAX_FD;i++)
			{
				SOCKET fd = fd_arr[i];
				if (FD_ISSET(fd, &fd_tmp))
				{
				}
			}*/
		}
	}
	return 0;
};

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
	{
		printf("init socket failed\n");
		return -1;
	}

	FD_ZERO(&fdwrite);
	for (int i=0;i<MAX_FD;i++)
	{
		SOCKADDR_IN udplocal;
		udplocal.sin_family = AF_INET;
		udplocal.sin_addr.s_addr = inet_addr("0.0.0.0");//htonl(ADDR_ANY)
		udplocal.sin_port = htons(LOCAL_BIND_PORT+2*i);

		SOCKET fd_client= socket(PF_INET, SOCK_DGRAM, IPPROTO_IP | IPPROTO_UDP);
		if (fd_client == INVALID_SOCKET)
		{
			printf("create socket() failed\n");
			return -1;
		}
		//int nbroadcast = 1;
		//setsockopt(fd_client, SOL_SOCKET, SO_BROADCAST, (char *)&nbroadcast, sizeof(int));
		int err = bind(fd_client, (SOCKADDR *)&udplocal, sizeof SOCKADDR_IN);
		if (err != 0)
		{
			printf("bind() socket failed\n");
			return -1;
		}

		//add multicast group 
		/*IP_MREQ mreq;
		mreq.imr_interface.s_addr = htonl(ADDR_ANY);
		mreq.imr_multiaddr.s_addr = inet_addr(MULTI_IP);
		setsockopt(fd_client, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof mreq);*/

		//set client socket ttl
		int ttl_value = 4;
		if (setsockopt(fd_client, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl_value, sizeof(ttl_value)) != 0)
		{
			printf("setsockopt multicast_ttl failed\n");
			return -1;
		}

		fd_arr[i] = fd_client;
		FD_SET(fd_client,&fdwrite);
	}
	::_beginthreadex(NULL,0,&threadfunc,NULL,0,NULL);
	printf("select io model client thread is running\n");
	getchar();
	for (int i=0;i<MAX_FD;i++)
	{
		//shutdown(fd_arr[i], 1);
		closesocket(fd_arr[i]);
	}
	WSACleanup();

	return 0;
}

