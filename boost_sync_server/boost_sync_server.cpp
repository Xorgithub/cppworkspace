// boost_sync_server.cpp : 定义控制台应用程序的入口点。
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
int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}

