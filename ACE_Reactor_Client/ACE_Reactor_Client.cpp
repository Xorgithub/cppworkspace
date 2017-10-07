// ACE_Reactor_Client.cpp : �������̨Ӧ�ó������ڵ㡣
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
	/// ����
	int open();

	/// ֹͣ����
	int close();
protected:
	/// �̺߳���
	virtual int svc();
};

int TTcpNetThread::open() { return this->activate(); }

int TTcpNetThread::close()
{
	ACE_Reactor::instance()->end_reactor_event_loop(); // ��ֹACE_Proactorѭ��

	this->wait(); // �ȴ������ֳ�

	return 0;
}

int TTcpNetThread::svc()
{
	// Proactor���¼�ѭ����ʼ
	while(!ACE_Reactor::instance()->event_loop_done())  
	{  
		ACE_Reactor::instance()->handle_events();   
	}

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Network fin\n")));

	return 0;
}

/**********************************************************************************************
��Socket����У��������¼�����"������"��"д����"��ͨ�����������¼��Ĳ���ַ�������ʵ��Socket�е��첽������

Socket����е��¼�������

��ǰ�������Ѿ����ܹ�����ACE��Ӧ������У��κζ�����������ACE_Event_Handler�࣬��ͨ����������Ӧ����¼���������ʵ����Ӧ�Ļص�����ġ���Socket����У�����ͨ����Ҫ���صĺ�����

1.handle_input()
��I/O���������UNIX�е��ļ����������ϵ��������ʱ����Ӧ���Զ��ص��÷�����

2.handle_output()
��I/O�豸����������п��ÿռ�ʱ����Ӧ���Զ��ص��÷�����

3.handle_close()
���¼��������е��¼���Reactor���Ƴ���ʱ����á� 

���⣬Ϊ��ʹReactor��ͨ��I/O����ҵ���Ӧ���¼���������������������get_handle()������ʹ��Reactor������I/O������¼��������Ĺ����� 
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
