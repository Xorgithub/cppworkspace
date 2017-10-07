// ACE_Proactor_Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "ace/Message_Queue.h"
#include "ace/Asynch_IO.h"
#include "ace/OS.h"
#include "ace/Proactor.h"
#include "ace/Asynch_Connector.h"
#include "ace/SOCK_SEQPACK_Association.h"

#pragma comment(lib,"ACEd.lib")

class Service_Handler : public ACE_Service_Handler
{
public:
	Service_Handler()
	{
		//ACE_OS::printf("Service_Handler constructed for connector \n");
	}
	~Service_Handler ()
	{
		if (this->handle () != ACE_INVALID_HANDLE)
			ACE_OS::closesocket (this->handle ());
		//ACE_OS::printf("one Service_Handler for connecter destructed");
	}

	void post_send(void)
	{
		do 
		{
			time_t now = ACE_OS::gettimeofday().sec();
			ACE_Message_Block *mb = new ACE_Message_Block(128);
			
			char buff[64];
			ACE_INET_Addr addr;
			ACE_SOCK_SEQPACK_Association ass=ACE_SOCK_SEQPACK_Association(this->handle());
			size_t addr_size=sizeof ACE_INET_Addr;
			ass.get_local_addrs(&addr,addr_size);
			//ACE_OS::printf("fd:%d ip:%d port:%d\n",(int)this->handle(), addr.get_ip_address(), addr.get_port_number());
			sprintf(buff,"%d",addr.get_port_number());
			mb->copy(buff/*ctime(now)*/);

			if (this->writer_.write(*mb,mb->length()) !=0)
			{
				ACE_OS::printf("Begin write fail in open\n");
				delete this;
				break;
			}
			else
			{
				ACE_OS::printf("sended:%s\n",mb->rd_ptr());
			}
		} while (0);
	}

	void post_recv(void)
	{
		do 
		{
			ACE_Message_Block *mb = new ACE_Message_Block(buffer,128);
			if (this->reader_.read (*mb, mb->space ()) != 0)
			{
				ACE_OS::printf("Begin read fail\n");
				delete this;
				break;
			}
		} while (0);
	}

	virtual void open (ACE_HANDLE h, ACE_Message_Block&)
	{
		do 
		{
			this->handle (h);
			if (this->writer_.open (*this) != 0 )
			{
				ACE_ERROR ((LM_ERROR, ACE_TEXT ("%p\n"),ACE_TEXT ("Service_Handler open")));
				delete this;
				break;
			}
			post_send();

			if (this->reader_.open (*this) != 0 )
			{
				ACE_ERROR ((LM_ERROR, ACE_TEXT ("%p\n"),ACE_TEXT ("Service_Handler open")));
				delete this;
				break;
			}
			post_recv();

		} while (0);
	}

	virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result &result)
	{
		result.message_block ().release();
		//ACE_OS::sleep(1);
		post_send();
	}

	virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result &result)
	{
		do 
		{
			ACE_Message_Block &mb = result.message_block ();
			if (!result.success () || result.bytes_transferred () == 0)
			{
				mb.release ();
				delete this;
				break;
			}
			ACE_OS::printf("received:%s\n",mb.rd_ptr());
			mb.release();

			post_recv();
		} while (0);
	}
private:
	ACE_Asynch_Write_Stream writer_;
	ACE_Asynch_Read_Stream reader_;
	char buffer[128];
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

#define TCP_CLIENT_THREAD_SEND	0x777

const int CLIENT_CONNECTION_NUM_OF_PER_THREAD = 1; //< 客户端每个线程的连接数

#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
/**
* @class TTcpClientThread
* @brief TCP客户端测试线程
*/
class TTcpClientThread : public ACE_Task<ACE_MT_SYNCH>
{
	ACE_SOCK_Connector connector[CLIENT_CONNECTION_NUM_OF_PER_THREAD]; //< 连接器
	ACE_SOCK_Stream peerStream[CLIENT_CONNECTION_NUM_OF_PER_THREAD]; //< 流对象

public:
	/// ctor
	~TTcpClientThread();
	
	/// 运行
	int open();

	/// 停止运行
	int close();
private:
	/// 线程函数
	virtual int svc();
};

TTcpClientThread::~TTcpClientThread()
{
	for(int i = 0; i < CLIENT_CONNECTION_NUM_OF_PER_THREAD; i++)
		peerStream[i].close();
}

int TTcpClientThread::open() { return this->activate(); }

int TTcpClientThread::close()
{
	ACE_TRACE("TTcpClientThread::close");

	ACE_Message_Block* termBlock;

	ACE_NEW_NORETURN(termBlock, ACE_Message_Block(0, ACE_Message_Block::MB_HANGUP));

	if (!termBlock)
		ACE_DEBUG((LM_ERROR, ACE_TEXT("Allocate failed %i"), errno));
	else
	{
		putq(termBlock);

		wait();
	}

	return 0;
}

int TTcpClientThread::svc()
{
	ACE_INET_Addr srvAddr(7878, "127.0.0.1");

	for(int i = 0; i < CLIENT_CONNECTION_NUM_OF_PER_THREAD; i++)
	{
		if (connector[i].connect(peerStream[i], srvAddr) == -1)
		{
			ACE_ERROR((LM_ERROR, ACE_TEXT("%i Failed to connect server errno=%i\n"), i, errno));
		}

		Sleep(100);
	}

	struct TPack
	{
#pragma pack(push)
#pragma pack(1)
		unsigned int seq;
		unsigned short len;
		char data [128];
#pragma pack(pop)
	};

	ACE_Message_Block* msg = 0;

	ACE_INET_Addr localAddr;
	ACE_TCHAR localAddrStr[128];

	peerStream[0].get_local_addr(localAddr);

	localAddr.addr_to_string(localAddrStr, sizeof(localAddrStr) / sizeof(ACE_TCHAR));

	TPack data;
	int len = sizeof(unsigned int) + sizeof(unsigned short);

	data.seq = 0;
	data.len = strlen(localAddrStr) + 1;
	strcpy(data.data, localAddrStr);

	len += data.len;

	char tmp[sizeof(TPack)];

	char buf[256];

	memcpy(tmp, &data, len);

	while(true) // 线程循环
	{
		if (getq(msg) != -1)
		{
			switch(msg->msg_type())
			{
			case ACE_Message_Block::MB_HANGUP:
				{
					msg->release();

					return 0;
				}
				break;
			default:
				{
					for(int i = 0; i < CLIENT_CONNECTION_NUM_OF_PER_THREAD; i++)
					{
						peerStream[i].send(tmp, 5);
						Sleep(100);
						peerStream[i].send(tmp + 5, len - 5);
						Sleep(100);

						ACE_Time_Value timeout(2);

						int recvLen =  peerStream[i].recv_n(buf, sizeof(unsigned int) + sizeof(unsigned short), 0, &timeout);

						if (recvLen == sizeof(unsigned int) + sizeof(unsigned short))
						{
							short dataLen = *(short *)(buf + 4);

							if (dataLen > 256)
								dataLen = 256;

							recvLen = peerStream[i].recv_n(buf, dataLen, 0, &timeout);

							if (recvLen != dataLen)
								ACE_DEBUG((LM_INFO, ACE_TEXT("Failed to recv data, length is %i, but only get %i\n"), dataLen, recvLen));
							else
								ACE_DEBUG((LM_INFO, ACE_TEXT("Client get data: len=%i data=%s\n"), recvLen, buf));
						} // if recvLen
					} // for
				} // default
				break;
			} // switch

			msg->release();
		} // if getq
	} // while

	ACE_DEBUG((LM_INFO, ACE_TEXT("Exit client thread")));

	return 0;
}

#include <vector>
#define CLIENT_THREAD_NUM 4
int main(int argc, char *argv[]) 
{

	ACE_INET_Addr remote_addr(4567,ACE_LOCALHOST); 

	std::vector<ACE_Asynch_Connector<Service_Handler> *> vtconnector;
	for (int i=0;i<2000;i++)
	{
		ACE_INET_Addr local_addr(10000+i,ACE_LOCALHOST); 
		ACE_Asynch_Connector<Service_Handler> *connector = new ACE_Asynch_Connector<Service_Handler>;
		connector->open();
		if (connector->connect(remote_addr,local_addr) == -1)
			return -1;
		vtconnector.push_back(connector);
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
	//ACE_Proactor::instance ()->proactor_run_event_loop();

	return 0; 
}
