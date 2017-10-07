#include "StdAfx.h"
#include "composite_impl.h"

#include <iostream> 

#include <algorithm> 

/*--------------------- 
Component 成员函数的实现 
---------------------*/ 
void Component::Add(Component* pChild) 
{ 
} 

void Component::Remove(Component* pChild) 
{ 
} 

Component* Component::GetChild(int nIndex) 
{ 
	return NULL; 
} 

/*--------------------- 
Leaf 成员函数的实现 
---------------------*/ 
void Leaf::Operation() 
{ 
	std::cout << "Operation by leaf "<<this<<std::endl; 
} 

/*--------------------- 
Composite 成员函数的实现 
---------------------*/ 
Composite::~Composite() 
{ 
	for (std::list<Component*>::iterator iter  = m_ListOfComponent.begin();
		iter != m_ListOfComponent.end(); ++iter) 
	{
		delete *iter;
	} 
} 

void Composite::Add(Component* pChild) 
{ 
	m_ListOfComponent.push_back(pChild); 
} 

void Composite::Remove(Component* pChild) 
{ 
	std::list<Component*>::iterator iter = find(m_ListOfComponent.begin(), m_ListOfComponent.end(), pChild); 
	if (m_ListOfComponent.end() != iter) 
	{ 
		m_ListOfComponent.erase(iter); 
	} 
} 

Component* Composite::GetChild(int nIndex) 
{ 
	if (nIndex <= 0 || nIndex > m_ListOfComponent.size()) 
		return NULL; 
	Component *pTmp = NULL;
	int index = 1;
	for (std::list<Component*>::iterator iter  = m_ListOfComponent.begin();
		iter != m_ListOfComponent.end(); ++iter,++index) 
	{ 
		if (index == nIndex)
		{
			pTmp = *iter;
			break;
		}
	} 
	return pTmp; 
} 

void Composite::Operation() 
{ 
	std::cout << "Operation by Composite "<<this<<std::endl; 
	std::list<Component*>::iterator iter1, iter2; 
	for (iter1  = m_ListOfComponent.begin(), iter2 = m_ListOfComponent.end(); iter1 != iter2; ++iter1) 
	{ 
		(*iter1)->Operation(); 
	}
} 