#include "StdAfx.h"
#include "bridge_impl.h"

#include "bridge_impl.h" 

#include <iostream> 

void ConcreateImplementorA::OperationImpl() 
{ 
	std::cout << "Implementation by ConcreateImplementorA\n"; 
} 

void ConcreateImplementorB::OperationImpl() 
{ 
	std::cout << "Implementation by ConcreateImplementorB\n"; 
} 

Abstraction::Abstraction(Implementor* pImplementor) 
: m_pImplementor(pImplementor) 
{ 
} 

Abstraction::~Abstraction() 
{ 
	delete m_pImplementor; 
	m_pImplementor = NULL; 
} 

void Abstraction::Operation() 
{ 
	m_pImplementor->OperationImpl(); 
} 
