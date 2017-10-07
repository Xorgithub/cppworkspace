// ACE_Reactor_Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <ace/Reactor.h>  
#include <ace/SOCK_Connector.h>   
#include <ace/SOCK_Acceptor.h>   
#include <ace/Auto_Ptr.h> 
#include "ace/OS.h"
#include "ace/Log_Msg.h"
#include <list>
#pragma comment(lib,"ACEd.lib")

class ClientService : public ACE_Event_Handler  
{  
public:  
	ACE_SOCK_Stream &peer (void) { return this->sock_; }  

	int regist_this(void)  
	{  
		//注册读就绪回调函数  
		return this->reactor ()->register_handler(this, ACE_Event_Handler::READ_MASK);  
	}  

	virtual ACE_HANDLE get_handle (void) const { return this->sock_.get_handle (); }  

	virtual int handle_input (ACE_HANDLE fd )  
	{  
		int rev = peer().recv(buf,sizeof(buf));  
		if(rev<=0)  
			return -1;  
		buf[rev] = '\0';  
		printf("recv:%s",buf);  

		return 0;  
	}  

	// 释放相应资源  
	virtual int handle_close (ACE_HANDLE, ACE_Reactor_Mask mask)  
	{  
		if (mask == ACE_Event_Handler::WRITE_MASK)  
			return 0;  
		mask = ACE_Event_Handler::ALL_EVENTS_MASK |  
			ACE_Event_Handler::DONT_CALL;  
		this->reactor ()->remove_handler (this, mask);  
		this->sock_.close ();  
		delete this;    //socket出错时，将自动删除该客户端，释放相应资源  
		return 0;  
	}  

protected:  
	char buf[100];  
	ACE_SOCK_Stream sock_;  
};  


class ClientAcceptor : public ACE_Event_Handler  
{  
public:  
	virtual ~ClientAcceptor (){this->handle_close (ACE_INVALID_HANDLE, 0);}  

	int start_listen (const ACE_INET_Addr &listen_addr)  
	{  
		if (this->acceptor_.open (listen_addr, 1) == -1)  
		{  
			printf("open port fail\n");  
			return -1;  
		}  
		//注册接受连接回调事件  
		return this->reactor ()->register_handler(this, ACE_Event_Handler::ACCEPT_MASK);  
	}  

	virtual ACE_HANDLE get_handle (void) const  
	{ return this->acceptor_.get_handle (); }  

	virtual int handle_input (ACE_HANDLE fd )  
	{  
		ClientService *client = new ClientService();  
		auto_ptr<ClientService> p (client);  

		if (this->acceptor_.accept (client->peer ()) == -1)  
		{  
			printf("accept client fail\n");  
			return -1;  
		}  
		p.release ();  
		client->reactor (this->reactor ());  
		if (client->regist_this () == -1)  
			client->handle_close (ACE_INVALID_HANDLE, 0);  
		return 0;  
	}  

	virtual int handle_close (ACE_HANDLE handle,  
		ACE_Reactor_Mask close_mask)  
	{  
		if (this->acceptor_.get_handle () != ACE_INVALID_HANDLE)  
		{  
			ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK |  
				ACE_Event_Handler::DONT_CALL;  
			this->reactor ()->remove_handler (this, m);  
			this->acceptor_.close ();  
		}  
		return 0;  
	}  

protected:  
	ACE_SOCK_Acceptor acceptor_;  
};  



int main1(int argc, char *argv[])   
{  
	ACE_INET_Addr addr(4567,"127.0.0.1");  
	ClientAcceptor server;  
	server.reactor(ACE_Reactor::instance());  
	server.start_listen(addr);  

	while(true)  
	{  
		ACE_Reactor::instance()->handle_events();   
	}  

	return 0;   
}

////////////////////////////////////////////////
#define MAX_BUFF_SIZE     1024
#define LISTEN_PORT       4567
#define SERVER_IP         ACE_LOCALHOST

class ClientHandler : public ACE_Event_Handler
{
public:
	friend class ServerAcceptor;
public:
	ClientHandler(){}
	~ClientHandler()
	{
		sock_stream.close();
		ACE_Reactor::instance()->remove_handler(this,ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL);
	}
	int send_some(const void *buff, int bytes)
	{
		return sock_stream.send(buff,bytes);
	}
	ACE_SOCK_Stream& GetStream(){return sock_stream;}    //给accept提供接口绑定数据通道
public:
	virtual int handle_input(ACE_HANDLE fd);    //I/O触发事件后调用
	virtual ACE_HANDLE get_handle(void) const {return sock_stream.get_handle();}    //不重载需要手动将handle传入ACE_Reactor
private:
	ACE_INET_Addr Cli_addr;
	ACE_SOCK_Stream sock_stream;
};

int ClientHandler::handle_input(ACE_HANDLE fd)
{
	char strBuffer[MAX_BUFF_SIZE];
	int byte = sock_stream.recv(strBuffer,MAX_BUFF_SIZE); //可读数据
	if (-1 == byte)
	{
		ACE_DEBUG((LM_INFO, ACE_TEXT("receive data failed\n")));
	}
	else if(0 == byte)
	{
		sock_stream.close();
		ACE_Reactor::instance()->remove_handler(this,ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL);
		ACE_DEBUG((LM_INFO, ACE_TEXT("client closed!\n")));
	}
	else
	{
		ACE_DEBUG((LM_INFO, ACE_TEXT("receive:%s\n"),strBuffer));
		
		sock_stream.send(strBuffer,strlen(strBuffer)+1);
	}
	return 0;
}

// ServerAcceptor
class ServerAcceptor : public ACE_Event_Handler
{
public:
	ServerAcceptor(int port,char* ip);
	~ServerAcceptor();
	virtual int handle_input(ACE_HANDLE fd);  // ACE框架回调
	virtual ACE_HANDLE get_handle(void) const {return Svr_aceept.get_handle();}
private:
	ACE_INET_Addr Svr_addr;
	ACE_SOCK_Acceptor Svr_aceept;
	std::list<ClientHandler*> m_streamPool;  //stream pool
};

ServerAcceptor::ServerAcceptor(int port,char* ip):Svr_addr(port,ip)
{
	if (-1 == Svr_aceept.open(Svr_addr,1))
	{
		ACE_DEBUG((LM_ERROR,ACE_TEXT("accept open failed\n")));
		Svr_aceept.close();
	}
	ACE_DEBUG((LM_ERROR,ACE_TEXT("accept open success\n")));
}

ServerAcceptor::~ServerAcceptor()
{
	ACE_Reactor::instance()->remove_handler(this,ACE_Event_Handler::ACCEPT_MASK);
	Svr_aceept.close();

	std::list<ClientHandler*>::iterator it;
	for (it = m_streamPool.begin();it != m_streamPool.end();++it)
	{
		if (NULL != (*it))
		{
			delete (*it);
		}
	}
}
#include "ace/SOCK_SEQPACK_Association.h"
int ServerAcceptor::handle_input(ACE_HANDLE fd )  
{
	ClientHandler *stream = new ClientHandler();    //产生新通道
	if (NULL != stream)
	{
		m_streamPool.push_back(stream);
	}
	if (Svr_aceept.accept(stream->GetStream()) == -1)  //绑定通道
	{  
		printf("accept client fail\n");  
		return -1;  
	}
	ACE_Reactor::instance()->register_handler(stream,ACE_Event_Handler::READ_MASK);  //通道注册到ACE_Reactor

	ACE_INET_Addr raddr;
	stream->GetStream().get_remote_addr(raddr);
	ACE_DEBUG ((LM_DEBUG,ACE_TEXT ( "client:%s %d\n" ),raddr.get_host_addr(),raddr.get_port_number()));

	/*ACE_INET_Addr addr;
	ACE_SOCK_SEQPACK_Association ass=ACE_SOCK_SEQPACK_Association(fd);
	size_t addr_size=sizeof ACE_INET_Addr;
	ass.get_remote_addrs(&addr,addr_size);
	ACE_OS::printf("fd:%d ip:%d port:%d\n",(int)fd, addr.get_ip_address(), addr.get_port_number());*/
	//ACE_DEBUG((LM_ERROR,ACE_TEXT("User connect success!\n")));
	return 0;
}  

#include <ace/OS.h>
#include <ace/Task.h>

class TTcpNetThread : public ACE_Task_Base
{
public:
	/// 运行
	int open();

	/// 停止运行
	int close();
protected:
	/// 线程函数
	virtual int svc();
};

int TTcpNetThread::open() { return this->activate(); }

int TTcpNetThread::close()
{
	ACE_Reactor::instance()->end_reactor_event_loop(); // 终止ACE_Proactor循环

	this->wait(); // 等待清理现场

	return 0;
}

int TTcpNetThread::svc()
{
	ACE_Reactor::instance()->run_reactor_event_loop();

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Network fin\n")));

	return 0;
}

#define CLIENT_THREAD_NUM 4

int main(int argc, char *argv[])
{
	ServerAcceptor server(LISTEN_PORT,(char *)SERVER_IP);

	//listen port注册到ACE_Reactor
	ACE_Reactor::instance()->register_handler(&server,ACE_Event_Handler::ACCEPT_MASK);

	TTcpNetThread netThread[CLIENT_THREAD_NUM];

	for(int i = 0; i < CLIENT_THREAD_NUM; i++)
	{
		netThread[i].open();
	}
	while (getchar())
	{
		ACE_OS::sleep(1);
	}

	//进入消息循环，有I/O事件回调handle_input
	//ACE_Reactor::instance()->run_reactor_event_loop();

	return 0;
}