#pragma once

#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Stream.h>
#include <ace/Reactor.h> 
#include <ace/Log_Msg.h>
#include <ace/Svc_Handler.h>
#include <ace/ACE.h>
#include <ace/OS.h>

class Server_svc_handler : public ACE_Svc_Handler <ACE_SOCK_STREAM,ACE_NULL_SYNCH>  
{  
public:  
	Server_svc_handler() 
	{  
		data= new char[DATA_SIZE];  
	}  
	int open(void*)  
	{  
		ACE_DEBUG((LM_DEBUG, "%P|%t, %s", " Connection established.\n")); //打印debug级别的log，%P%t表示进程号和线程号，%s是要打印的消息。  
		ACE_Reactor::instance()->register_handler(this,  ACE_Event_Handler::READ_MASK); //注册socket上可读事件的处理器，当当前的socket上有可读事件时，就会触发这个handler的handle_input方法。  
		char* msg = "Client connected successfully in server.";  
		peer().send(msg, strlen(msg)+1);  
		return 0;  
	}  

	int handle_input(ACE_HANDLE)  
	{  
		ssize_t cnt = peer().recv(data, DATA_SIZE);  
		if (cnt <= 0)  
		{  
			ACE_DEBUG((LM_WARNING, " No data received.\n")); //打印warn级别日志  
		}  
		else  
		{  
			ACE_DEBUG((LM_DEBUG, "recv:%s\n", data));  
			//在这里添加具体业务处理代码  
			//char* msg = "Request is processed successfully"; 
			char msg[64];
			sprintf(msg,"http %s 200 OK",data);
			peer().send(msg, strlen(msg)+1);  
		}  
		return 0;  
	}  

private:  
	char* data;  
	static const int DATA_SIZE = 64;  
};  
