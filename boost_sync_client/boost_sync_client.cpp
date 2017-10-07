// boost_sync_client.cpp : 定义控制台应用程序的入口点。
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
int tcp_sync_client(const char *ip,unsigned short port)
{
	boost::asio::io_service service;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip), port);
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
			/*int ret = sock.send(boost::asio::buffer("hello i am client"));
			if (ret < 0)
			{
				printf("send failed:%d\n",ret);
				return ret;
			}*/
		}
	}
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}

