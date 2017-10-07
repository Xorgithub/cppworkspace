#include "StdAfx.h"
#include "chainofresponsibility_impl.h"

#include <iostream> 

Handler::Handler(Handler *pNextHandler /* = NULL*/) 
: m_pNextHandler(pNextHandler) 
{ 
} 

Handler::~Handler() 
{ 
	delete m_pNextHandler; 
	m_pNextHandler = NULL; 
} 

void ConcreateHandler1::HandleRequest() 
{ 
	if (m_pNextHandler) 
	{ 
		std::cout <<"HandleRequest not by ConcreateHandler1,Transfer Next\n";
		m_pNextHandler->HandleRequest(); 
	} 
	else 
	{ 
		std::cout << "HandleRequest by ConcreateHandler1\n"; 
	} 
}

void ConcreateHandler2::HandleRequest() 
{ 
	if (m_pNextHandler) 
	{ 
		std::cout <<"HandleRequest not by ConcreateHandler2,Transfer Next\n";
		m_pNextHandler->HandleRequest(); 
	} 
	else 
	{ 
		std::cout << "HandleRequest by ConcreateHandler2\n"; 
	} 
} 
