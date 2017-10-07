// udp_sendrecv.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/pool/singleton_pool.hpp>

#include <winsock2.h>
#include <mswsock.h>
#include <MSTcpIP.h>
#include <errno.h>
#include <ws2ipdef.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

using namespace boost::asio;
int tcp_sync_client()
{
	boost::asio::io_service service;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("0.0.0.0"), 5000);
	boost::asio::ip::tcp::socket sock(service);
	try
	{
		sock.connect(ep);
	}
	catch (const boost::system::system_error& e)
	{
		return e.code().value();
	}

	int ret = sock.send(boost::asio::buffer("hello i am client"));
	if (ret < 0)
	{
		return ret;
	}
	while (1)
	{
		char data[512];
		int len = sock.read_some(boost::asio::buffer(data));
		if (len > 0)
		{
			printf("recv data:%s\n",data);
		}
	}
	return 0;
}

void connect_handler(const boost::system::error_code &ec)
{
	printf("connect success\n");
}
void tcp_async_client()
{
	boost::asio::io_service service;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("0.0.0.0"), 6000);
	boost::asio::ip::tcp::socket sock(service);
	sock.async_connect(ep,connect_handler);
	//boost::thread(boost::bind());
	service.run();
}

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;
typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_ptr;
typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;

void tcp_async_post_read(sock_ptr sock);
void tcp_async_receive_handler(sock_ptr sock, void *buf, 
							   const boost::system::error_code& ec, 
							   std::size_t bytes_transferred)
{
	printf("buff:%s len:%d\n",buf,bytes_transferred);

	tcp_async_post_read(sock);
}
void tcp_async_post_read(sock_ptr sock)
{
	char buf[512];
	int len = 512;
	sock->async_read_some(boost::asio::buffer(buf, len), 
		boost::bind(tcp_async_receive_handler, sock, buf, 
		boost::asio::placeholders::error, 
		boost::asio::placeholders::bytes_transferred));
}
void tcp_async_send_handler(sock_ptr sock,
							const boost::system::error_code& ec, 
							std::size_t bytes_transferred)
{
	printf("send %d bytes success\n",bytes_transferred);
}
void tcp_async_post_write(sock_ptr sock, const void *buf, size_t len)
{
	try
	{
		sock->async_write_some(boost::asio::buffer(buf, len), 
			boost::bind(tcp_async_send_handler, sock, 
			boost::asio::placeholders::error, 
			boost::asio::placeholders::bytes_transferred));
	}
	catch (const boost::system::system_error& e)
	{
		e.code().value();
	}
}

void handler_accept(sock_ptr sock, const boost::system::error_code &err)
{
	if (err)
	{
		return;
	}
	//post one read
	tcp_async_post_read(sock);
}

void start_accept(acceptor_ptr acc,sock_ptr sock,io_service_ptr service)
{
	boost::arg<1> arg_1;
	acc->async_accept(*sock,boost::bind(handler_accept,sock,_1));
}

void tcp_async_server()
{
	io_service_ptr service(new boost::asio::io_service);//create iocp
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(),6000);
	acceptor_ptr acc(new boost::asio::ip::tcp::acceptor(*service,ep));
	sock_ptr sock(new ip::tcp::socket(*service));
	start_accept(acc, sock, service);//post one accept

	service->run();
}

void client_session(sock_ptr sock)
{
	while (1)
	{
		char data[512];
		size_t len = sock->read_some(boost::asio::buffer(data,512));
		if (len > 0)
		{
			write(*sock,boost::asio::buffer("ok",2));
			sock->write_some(boost::asio::buffer("ok",2));
		}
	}
}

void tcp_sync_sever()
{
	boost::asio::io_service service;
	boost::asio::ip::tcp::endpoint ep(ip::tcp::v4(), 5000);
	boost::asio::ip::tcp::acceptor acc(service, ep);
	while (1)
	{
		sock_ptr sock(new ip::tcp::socket(service));
		acc.accept(*sock);
		boost::thread(boost::bind(client_session, sock));
	}
}

#define MAX_FD 16
fd_set fdread;
SOCKET fd_arr[MAX_FD] = {0};
unsigned int __stdcall threadfunc(void *)
{
	while (1)
	{
		fd_set fd_tmp = fdread;
		timeval tmv;
		tmv.tv_sec = 1;
		tmv.tv_usec = 0;
		int ret = select(0, &fd_tmp, NULL, NULL, &tmv);
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
					SOCKADDR_IN client_addr;
					int nlen_addr = sizeof(SOCKADDR_IN);
					char buff[40960]={0};
					int nrecv = recvfrom(fd, buff, 40960, 0, (SOCKADDR *)&client_addr, &nlen_addr);
					if (nrecv < 0)
					{
						printf("recvfrom error:%d\n", WSAGetLastError());
						continue;
					}
					printf("sock:%d src addr:%s:%d recv data:%s\n", 
						fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),buff);
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

#define MULTI_IP "239.0.0.37"

#define RECV_START_PORT 16000

#define	LOCAL_BIND_PORT 20260

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
	{
		printf("init socket failed\n");
		return -1;
	}

	FD_ZERO(&fdread);
	for (int i=0;i<MAX_FD;i++)
	{
		SOCKADDR_IN udplocal;
		udplocal.sin_family = AF_INET;
		udplocal.sin_addr.s_addr = inet_addr("0.0.0.0");//htonl(ADDR_ANY)
		udplocal.sin_port = htons(RECV_START_PORT+2*i);

		SOCKET fd_local= socket(PF_INET, SOCK_DGRAM, IPPROTO_IP | IPPROTO_UDP);
		if (fd_local == INVALID_SOCKET)
		{
			printf("create socket() failed\n");
			return -1;
		}
		//int nbroadcast = 1;
		//setsockopt(fd_local, SOL_SOCKET, SO_BROADCAST, (char *)&nbroadcast, sizeof(int));
		int err = bind(fd_local, (SOCKADDR *)&udplocal, sizeof SOCKADDR_IN);
		if (err != 0)
		{
			printf("bind() socket failed\n");
			return -1;
		}
		//char buff[1024]={0};
		//memset(buff, 0x0, 1024);

		//add multicast group 
		IP_MREQ mreq;
		mreq.imr_interface.s_addr = htonl(ADDR_ANY);
		mreq.imr_multiaddr.s_addr = inet_addr(MULTI_IP);
		setsockopt(fd_local, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof mreq);

		//set client socket ttl
		int ttl_value = 4;
		if (setsockopt(fd_local, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl_value, sizeof(ttl_value)) != 0)
		{
			printf("setsockopt multicast_ttl failed\n");
			return -1;
		}

		fd_arr[i] = fd_local;
		
		FD_SET(fd_local,&fdread);
	}
	
	::_beginthreadex(NULL,0,&threadfunc,NULL,0,NULL);
	printf("select io model server thread start running\n");
	getchar();
	for (int i=0;i<MAX_FD;i++)
	{
		//shutdown(fd_arr[i], 1);
		closesocket(fd_arr[i]);
	}
	WSACleanup();

	return 0;
}

