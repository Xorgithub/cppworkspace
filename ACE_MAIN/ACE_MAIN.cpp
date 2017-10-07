// ACE_MAIN.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "Server_svc_handler.h"
#include <ace/OS_NS_sys_select.h>
#include <ace/Acceptor.h>
#include <ace/ACE.h>
#include <ace/OS.h>
#ifdef _DEBUG
#pragma  comment (lib,"aced.lib")
#else
#pragma  comment (lib,"ace.lib")
#endif

typedef ACE_Acceptor<Server_svc_handler, ACE_SOCK_ACCEPTOR> ServerAcceptor;

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

int main(int argc, char* argv[])  
{
	ACE_DEBUG((LM_DEBUG,"Test server running...\n"));  
	const int PORT_NUM = 4567;  
	ACE_INET_Addr addr(PORT_NUM);  
	ServerAcceptor acceptor(addr, ACE_Reactor::instance());

	TTcpNetThread netThread[CLIENT_THREAD_NUM];

	for(int i = 0; i < CLIENT_THREAD_NUM; i++)
	{
		netThread[i].open();
	}
	while (getchar())
	{
		ACE_OS::sleep(1);
	}

	//ACE_Reactor::run_event_loop(); //会一直不停的监听注册的事件。  
	return 0;  
};  

////////////////client///////////////////////////////////////
#include "Client_svc_handler.h"
#include <ace/Connector.h>
#include "ace/SOCK_Connector.h"
//typedef Agent_svc_handler Client_svc_handler;
typedef ACE_Connector<Agent_svc_handler, ACE_SOCK_CONNECTOR> ClientConnector;  
int main2(int argc, char* argv[])  
{  
	const int SERVER_PORT_NUM = 12345;  
	ACE_INET_Addr remoteAddr(SERVER_PORT_NUM, "localhost");  
	ClientConnector connector;  
	Agent_svc_handler *handler= new Agent_svc_handler;  
	if(connector.connect(handler, remoteAddr) == -1 )  
	{  
		ACE_ERROR((LM_ERROR, "%P|%t, %s", "Connection failed"));  
	}  
	ACE_Reactor::run_event_loop();  
	return 0;  
}; 