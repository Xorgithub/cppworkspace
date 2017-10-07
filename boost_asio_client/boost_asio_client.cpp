// boost_asio_client.cpp : 定义控制台应用程序的入口点。
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
int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}

