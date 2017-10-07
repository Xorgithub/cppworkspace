// ACE_Reactor_Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "ace/Reactor.h"  
#include "ace/SOCK_Connector.h"  
#include "ace/OS.h"
#include "ace/Log_Msg.h"
#include <string>  
#include <iostream>  
using namespace std;  

class MyClient:public ACE_Event_Handler   
{  
public:  
	bool do_connect(string ip, int port, int local_port)  
	{  
		ACE_SOCK_Connector connector;  
		ACE_INET_Addr local_addr(local_port,"0.0.0.0");

		ACE_INET_Addr addr(port,ip.c_str());  
		ACE_Time_Value timeout(5,0);  
		if(connector.connect(peer_sock,addr,&timeout, local_addr) != 0)  
		{  
			cout<<"connect fail."<<endl;  
			return false;  
		}  
		int ret = ACE_Reactor::instance()->register_handler(this,ACE_Event_Handler::READ_MASK);  
		if (ret != 0)
		{
			cout<<"local_port:"<<local_port<<" register_handler fail."<<endl;
			return false;
		}
		sprintf(buf,"%d",local_port); 
		peer_sock.send(buf,strlen(buf)+1);  
		return true;  
	}  

	ACE_HANDLE get_handle(void) const  
	{  
		return peer_sock.get_handle();  
	}  

	int handle_input (ACE_HANDLE fd)  
	{  
		int rev=0;  
		ACE_Time_Value timeout(5,0);  
		if((rev=peer_sock.recv(buf,sizeof(buf),&timeout))>0)  
		{  
			buf[rev]='\0';  
			cout<<"recv: "<<buf<<endl;  
		} 
		ACE_INET_Addr raddr;
		peer_sock.get_local_addr(raddr);
		//ACE_DEBUG ((LM_DEBUG,ACE_TEXT ( " (%P|%t) close:%s %d\n " ),raddr.get_host_addr(),raddr.get_port_number()));
		sprintf(buf,"%d",raddr.get_port_number()); 
		peer_sock.send(buf,strlen(buf)+1); 
		
		return 0;  
	}  

private:  
	ACE_SOCK_Stream peer_sock;
	char buf[100];  
};

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
	// Proactor的事件循环开始
	while(!ACE_Reactor::instance()->event_loop_done())  
	{  
		ACE_Reactor::instance()->handle_events();   
	}

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Network fin\n")));

	return 0;
}

/**********************************************************************************************
在Socket编程中，常见的事件就是"读就绪"，"写就绪"，通过对这两个事件的捕获分发，可以实现Socket中的异步操作。

Socket编程中的事件处理器

在前面我们已经介绍过，在ACE反应器框架中，任何都必须派生自ACE_Event_Handler类，并通过重载其相应会调事件处理函数来实现相应的回调处理的。在Socket编程中，我们通常需要重载的函数有

1.handle_input()
当I/O句柄（比如UNIX中的文件描述符）上的输入可用时，反应器自动回调该方法。

2.handle_output()
当I/O设备的输出队列有可用空间时，反应器自动回调该方法。

3.handle_close()
当事件处理器中的事件从Reactor中移除的时候调用。 

此外，为了使Reactor能通过I/O句柄找到对应的事件处理器，还必须重载其get_handle()方法以使得Reactor建立起I/O句柄和事件处理器的关联。 
***********************************************************************************************/
#pragma comment(lib,"ACEd.lib")

#define CLIENT_THREAD_NUM 4

int main(int argc, char *argv[])   
{  
	for (int i=0;i<2000;i++)
	{
		MyClient *client = new MyClient;  
		if (!client->do_connect("127.0.0.1",4567,10000+i))
			break; 
	}

	system("pause");
	TTcpNetThread netThread[CLIENT_THREAD_NUM];

	for(int i = 0; i < CLIENT_THREAD_NUM; i++)
	{
		netThread[i].open();
	}
	while (getchar())
	{
		ACE_OS::sleep(1);
	}

	/*while(true)  
	{  
		ACE_Reactor::instance()->handle_events();   
	}*/ 

	return 0;   
}  
