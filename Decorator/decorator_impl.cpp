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
	m_pComponent->Operation(); //componetԭ��ְ��
	AddedBehavior(); //decorator��̬���ְ��
} 

void ConcreateDecorator::AddedBehavior() 
{ 
	std::cout << "AddedBehavior of ConcreateDecorator\n"; 
} 