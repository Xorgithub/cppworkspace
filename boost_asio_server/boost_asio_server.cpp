// boost_asio_server.cpp : 定义控制台应用程序的入口点。
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

#include <map>
using namespace std;

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;
typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_ptr;
typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;

map<int,sock_ptr> link_mgr_map;

void tcp_async_post_read(sock_ptr sock);
void tcp_async_post_write(sock_ptr sock, const void *buf, size_t len);
void tcp_async_receive_handler(sock_ptr sock, void *buf, 
							   const boost::system::error_code& ec, 
							   std::size_t bytes_transferred)
{
	printf("buff:%s len:%d\n",buf,bytes_transferred);

	tcp_async_post_write(sock,buf,bytes_transferred);

	tcp_async_post_read(sock);
}

char buf[512];
int len = 512;
void tcp_async_post_read(sock_ptr sock)
{
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

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
typedef boost::shared_ptr<ip::tcp::acceptor> acceptor_ptr;

void handle_accept(io_service *service,acceptor_ptr acc,socket_ptr sock, const boost::system::error_code &
				   err);
void start_accept(io_service *service,acceptor_ptr acc) 
{
	socket_ptr sock(new ip::tcp::socket(*service));
	acc->async_accept(*sock, boost::bind( handle_accept, service,acc,sock, _1) );
}
void handle_accept(io_service *service,acceptor_ptr acc,socket_ptr sock, const boost::system::error_code &
				   err) 
{
	if ( err) return;
	// 从这里开始, 你可以从socket读取或者写入
	static int index = 0;
	link_mgr_map.insert(make_pair(index++,sock));

	tcp_async_post_read(sock);

	start_accept(service,acc);
}

#define CLIENT_THREAD_NUM 4
#include <boost/threadpool/pool.hpp>

void work_thread_func(io_service *service)
{
	service->run();
}

int _tmain(int argc, _TCHAR* argv[])
{
	
	
	io_service service;
	ip::tcp::endpoint ep( ip::tcp::v4(), 4567); // 监听端口2001
	acceptor_ptr acc(new boost::asio::ip::tcp::acceptor(service,ep));
	start_accept(&service,acc);
	//service.run();
	
	boost::threadpool::pool tp(4);
	tp.schedule( boost::bind(work_thread_func, &service));
	while (getchar())
	{
		boost::this_thread::sleep(boost::posix_time::seconds(1));
	}

	return 0;
}

