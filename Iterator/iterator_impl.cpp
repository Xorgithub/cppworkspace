#include "StdAfx.h"
#include "iterator_impl.h"

#include <iostream> 
template <class DATA, int MAX_SIZE>
ConcreateAggregate<DATA, MAX_SIZE>::ConcreateAggregate()
:m_begin(NULL)
,m_end(NULL)
{
} 
template <class DATA, int MAX_SIZE>
ConcreateAggregate<DATA, MAX_SIZE>::~ConcreateAggregate() 
{ 
} 
template <class DATA, int MAX_SIZE>
ConcreateIterater<DATA>& ConcreateAggregate<DATA, MAX_SIZE>::begin()
{
	if (m_begin == NULL)
	{
		m_begin = new ConcreateIterater<DATA>(this);
		m_begin->m_nIndex = 0;
	}
	
	return *m_begin;
}
template <class DATA, int MAX_SIZE>
ConcreateIterater<DATA>& ConcreateAggregate<DATA, MAX_SIZE>::end()
{
	if (m_end == NULL)
	{
		m_end = new ConcreateIterater<DATA>(this);
		m_end->m_nIndex = m_aData.size();
	}

	return *m_end;
}
template <class DATA, int MAX_SIZE>
void ConcreateAggregate<DATA, MAX_SIZE>::push_back(DATA dt)
{
	m_aData.push_back(dt);
}
template <class DATA, int MAX_SIZE>
DATA ConcreateAggregate<DATA, MAX_SIZE>::pop_back()
{
	DATA tmp = m_aData.back();
	m_aData.pop_back();
	return tmp;
}
template <class DATA, int MAX_SIZE>
int ConcreateAggregate<DATA, MAX_SIZE>::GetSize() 
{ 
	return m_aData.size(); 
} 
template <class DATA, int MAX_SIZE>
DATA ConcreateAggregate<DATA, MAX_SIZE>::GetItem(int nIndex) 
{ 
	if (nIndex < m_aData.size()) 
	{ 
		return m_aData[nIndex]; 
	} 
	return -1; 
} 
