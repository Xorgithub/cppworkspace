#include "StdAfx.h"
#include "command_impl.h"

#include <iostream> 

void Receiver::Action() 
{ 
	std::cout << "Receiver Action\n"; 
} 

Invoker::Invoker(Command *pCommand) 
: m_pCommand(pCommand) 
{ 
} 

Invoker::~Invoker() 
{ 
	delete m_pCommand; 
	m_pCommand = NULL; 
} 

void Invoker::Invoke() 
{ 
	if (NULL != m_pCommand) 
	{ 
		m_pCommand->Execute(); 
	} 
} 

ConcreateComand::ConcreateComand(Receiver* pReceiver) 
: m_pReceiver(pReceiver) 
{ 
} 

ConcreateComand::~ConcreateComand() 
{ 
	delete m_pReceiver; 
	m_pReceiver = NULL; 
} 

void ConcreateComand::Execute() 
{ 
	if (NULL != m_pReceiver) 
	{ 
		m_pReceiver->Action(); 
	} 
	std::cout << "Execute by ConcreateComand\n"; 
} 
