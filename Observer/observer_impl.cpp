#include "StdAfx.h"
#include "observer_impl.h"

#include <iostream> 
#include <algorithm> 
/* ---------------------- 
|   Subject 类成员函数的实现 
| 
-*/ 
void Subject::Attach(Observer *pObserver) 
{ 
	std::cout << "Attach an Observer:"<<pObserver<<std::endl; 
	m_ListObserver.push_back(pObserver); 
}

void Subject::Detach(Observer *pObserver) 
{ 
	std::list<Observer*>::iterator iter; 
	iter = std::find(m_ListObserver.begin(), m_ListObserver.end(), pObserver); 
	if (m_ListObserver.end() != iter) 
	{ 
		std::cout << "Detach an Observer:"<<*iter<<std::endl; 
		m_ListObserver.erase(iter); 
	} 
} 

void Subject::Notify() 
{ 
	std::cout <<this<<" Notify Observers's State"<<std::endl; 
	std::list<Observer*>::iterator iter1, iter2; 
	for (iter1 = m_ListObserver.begin(), iter2 = m_ListObserver.end(); iter1 != iter2; ++iter1) 
	{ 
		(*iter1)->Update(this); 
	} 
} 

void Subject::SetState(STATE nState) 
{ 
	m_nSubjectState = nState; 
} 

STATE Subject::GetState() 
{ 
	return m_nSubjectState; 
} 

Subject::~Subject() 
{ 
	std::list<Observer*>::iterator iter1, iter2, temp; 
	for (iter1 = m_ListObserver.begin(), iter2 = m_ListObserver.end(); iter1 != iter2;) 
	{ 
		temp = iter1; 
		++iter1; 
		delete (*temp); 
	} 
	m_ListObserver.clear(); 
} 

/* ---------------------- 
|  ConcreateSubject 类成员函数的实现 
| 
-*/ 
void ConcreateSubject::SetState(STATE nState) 
{ 
	m_nSubjectState = nState; 
} 

STATE ConcreateSubject::GetState() 
{ 
	return m_nSubjectState; 
} 

/* ---------------------- 
|  ConcreateObserver 类成员函数的实现 
| 
-*/ 
void ConcreateObserver::Update(Subject* pSubject) 
{
	if (NULL == pSubject) 
		return; 
	m_nObserverState = pSubject->GetState(); 
	std::cout << "Observer:"<<this<<" GetState:" <<m_nObserverState << std::endl; 
} 
