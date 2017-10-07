// ACE_Proactor_Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "ace/Asynch_IO.h"
#include "ace/OS_main.h"
#include "ace/Proactor.h"
#include "ace/Asynch_Acceptor.h"
#include "ace/INET_Addr.h"
#include "ace/OS.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ace/Message_Block.h"
#include "ace/Containers.h"
#include "ace/SOCK_SEQPACK_Association.h"

ACE_DLList<ACE_Asynch_Write_Stream> wList;

class Service_Handler:public ACE_Service_Handler
{
public:
	Service_Handler()
	{

	}
	~Service_Handler(void)
	{
		if(this->handle()!=ACE_INVALID_HANDLE)
			ACE_OS::closesocket(this->handle());
	}

	virtual void open(ACE_HANDLE h,ACE_Message_Block &message_block)
	{
		//handle_= h;
		//this->handle(h);
		if(rs_.open(*this,h))
		{
			ACE_ERROR ((LM_ERROR,"%p/n","ACE_Asynch_Read_Stream::open"));
			return;
		}

		if(ws_.open(*this))
		{
			ACE_ERROR ((LM_ERROR,"%p/n","ACE_Asynch_Write_Stream::open"));
			return;
		}

		if (post_recv()==-1)
			return;

		//wList.insert_tail(&ws_);
		addresses(remote_address,local_address);
		remote_address.addr_to_string(peer_name,MAXHOSTNAMELEN);

		ACE_INET_Addr addr;
		ACE_SOCK_SEQPACK_Association ass=ACE_SOCK_SEQPACK_Association(h);
		size_t addr_size=sizeof ACE_INET_Addr;
		ass.get_remote_addrs(&addr,addr_size);
		ACE_OS::printf("fd:%d ip:%d port:%d\n",(int)h, addr.get_ip_address(), addr.get_port_number());
		//ACE_DEBUG((LM_DEBUG,ACE_TEXT("peer:%s\n"),peer_name));
	}
protected:
	int post_recv(void)
	{
		ACE_Message_Block *mb=0;
		ACE_NEW_RETURN(mb,ACE_Message_Block(512),-1);

		if(rs_.read(*mb,mb->space())==-1)
		{
			ACE_ERROR_RETURN((LM_ERROR,"%p/n","ACE_Asynch_Read_Stream::read"),-1);
		}
		return 0;
	}
	virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result &result)
	{
		//ACE_HANDLE h = result.handle();
		ACE_Message_Block &mb = result.message_block ();
		if (result.success()&&result.bytes_transferred()!=0)
		{
			ACE_DEBUG((LM_DEBUG,ACE_TEXT("recv:%s\n"),mb.rd_ptr()));
			if (ws_.write(*mb.duplicate(),result.message_block().length())==-1)
			{
				ACE_ERROR ((LM_ERROR,"%p\n","ACE_Asynch_Write_Stream::write"));
			}
			/*ACE_DLList_Iterator<ACE_Asynch_Write_Stream> iter(wList);
			while(!iter.done())
			{
				if (iter.next()->write(*result.message_block().duplicate(),
					result.message_block().length())==-1)
				{
					ACE_ERROR ((LM_ERROR,"%p/n","ACE_Asynch_Write_Stream::write"));
				}
				iter++;
			}*/
			mb.release();
			post_recv();
		}
		else
		{
			mb.release();
			/*ACE_DLList_Iterator<ACE_Asynch_Write_Stream> iter(wList);
			while (!iter.done ())
			{
				if(&ws_==iter.next())
				{
					iter.remove();
					break;
				}
				iter++;
			}*/
			delete this;
		}
	}
	virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result &result)
	{
		//ACE_OS::printf("write complete:%d %d\n", result.success(),result.bytes_transferred());
		result.message_block().release();
	}
private:
	ACE_Asynch_Read_Stream rs_;
	ACE_Asynch_Write_Stream ws_;

	ACE_HANDLE handle_;

	ACE_TCHAR peer_name[MAXHOSTNAMELEN];
	ACE_INET_Addr remote_address;
	ACE_INET_Addr local_address;
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
	ACE_Proactor::instance()->proactor_end_event_loop(); // 终止ACE_Proactor循环

	this->wait(); // 等待清理现场

	return 0;
}

int TTcpNetThread::svc()
{
	/*
	ACE_INET_Addr listenAddr(4567); // 默认监听地址
	TTcpAcceptor tcpAcceptor; // 接收器

	// 演出开始
	if (tcpAcceptor.open(listenAddr, 0, 1, 5, 1, 0, 0) != 0)
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("failed to open TcpAcceptor errno=%i\n"), errno), -1);
	*/
	// Proactor的事件循环开始
	ACE_Proactor::instance()->proactor_run_event_loop();

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Network fin\n")));

	return 0;
}

#pragma comment(lib,"ACEd.lib")

#define CLIENT_THREAD_NUM 4

int main(int argc,char *argv[])
{
	ACE_Asynch_Acceptor<Service_Handler> acceptor;

	if(acceptor.open(ACE_INET_Addr(4567),0,1) == -1)
	{
		return -1;
	}

	TTcpNetThread netThread[CLIENT_THREAD_NUM];

	for(int i = 0; i < CLIENT_THREAD_NUM; i++)
	{
		netThread[i].open();
	}
	while (getchar())
	{
		ACE_OS::sleep(1);
	}
	//ACE_Proactor::instance()->proactor_run_event_loop();

	return 0;
};
