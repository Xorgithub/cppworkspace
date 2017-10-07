#include "StdAfx.h"
#include "singleton_impl.h"
#include <iostream> 

// ��ľ�̬��Ա����Ҫ����������ж��� 
Singleton* Singleton::m_pStatic = NULL; 

Singleton* Singleton::GetInstancePtr() 
{
	if (NULL == m_pStatic) 
	{ 
		m_pStatic = new Singleton(); 
	} 
	return m_pStatic; 
} 

Singleton Singleton::GetInstance() 
{ 
	return *GetInstancePtr(); 
} 

void Singleton::Test() 
{ 
	std::cout << "Test!\n"; 
} 
