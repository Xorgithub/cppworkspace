#pragma once

#include "stdafx.h" 

#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Stream.h>
#include <ace/Reactor.h> 
#include <ace/Log_Msg.h>
#include <ace/Svc_Handler.h>
#include <ace/ACE.h>
#include <ace/OS.h>

class Agent_svc_handler : public ACE_Svc_Handler <ACE_SOCK_STREAM, ACE_NULL_SYNCH>  
{  
public:  
	Agent_svc_handler()  
	{  
		data= new char[DATA_SIZE];  
	}  
	int handle_input(ACE_HANDLE)  
	{  
		peer().recv(data, DATA_SIZE);  
		ACE_DEBUG((LM_DEBUG, "%s", data));  
		return 0;  
	}
private:  
	char* data;  
	static const int DATA_SIZE = 64;  
};  
