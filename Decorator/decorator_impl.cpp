#include "StdAfx.h"
#include "decorator_impl.h"

#include <iostream> 

Decorator::~Decorator() 
{ 
	delete m_pComponent; 
	m_pComponent = NULL; 
} 

void ConcreateComponent::Operation() 
{ 
	std::cout << "Operation of ConcreateComponent\n"; 
} 

void ConcreateDecorator::Operation() 
{ 
	m_pComponent->Operation(); //componet原有职责
	AddedBehavior(); //decorator动态添加职责
} 

void ConcreateDecorator::AddedBehavior() 
{ 
	std::cout << "AddedBehavior of ConcreateDecorator\n"; 
} 