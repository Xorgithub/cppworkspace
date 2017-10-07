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
		ACE_DEBUG((LM_DEBUG, "%P|%t, %s", " Connection established.\n")); //��ӡdebug�����log��%P%t��ʾ���̺ź��̺߳ţ�%s��Ҫ��ӡ����Ϣ��  
		ACE_Reactor::instance()->register_handler(this,  ACE_Event_Handler::READ_MASK); //ע��socket�Ͽɶ��¼��Ĵ�����������ǰ��socket���пɶ��¼�ʱ���ͻᴥ�����handler��handle_input������  
		char* msg = "Client connected successfully in server.";  
		peer().send(msg, strlen(msg)+1);  
		return 0;  
	}  

	int handle_input(ACE_HANDLE)  
	{  
		ssize_t cnt = peer().recv(data, DATA_SIZE);  
		if (cnt <= 0)  
		{  
			ACE_DEBUG((LM_WARNING, " No data received.\n")); //��ӡwarn������־  
		}  
		else  
		{  
			ACE_DEBUG((LM_DEBUG, "recv:%s\n", data));  
			//��������Ӿ���ҵ�������  
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
