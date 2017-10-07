#pragma once

#ifndef ITERATOR_H 

#define ITERATOR_H 

#include <vector>
#include <iostream>
using namespace std;

class Iterater; 

// 容器的抽象基类
template <class DATA> class Aggregate 
{ 
public: 
	virtual ~Aggregate(){} 
	virtual int GetSize() = 0; 
	virtual DATA GetItem(int nIndex) = 0; 
}; 

// 迭代器的抽象基类 
class Iterater 
{ 
public: 
	virtual ~Iterater(){} 
	virtual Iterater& operator++() = 0;
	virtual Iterater& operator++(int) = 0;
private: 
}; 

// 一个具体的容器类,这里是用数组表示
template <class DATA> class ConcreateIterater;

template <class DATA=int, typename MAX_SIZE=1024> class ConcreateAggregate : public Aggregate<DATA> 
{ 
public: 
	ConcreateAggregate(); 
	virtual ~ConcreateAggregate(); 
	virtual ConcreateIterater<DATA>& begin();
	virtual ConcreateIterater<DATA>& end();
	virtual void push_back(DATA dt);
	virtual DATA pop_back();
	virtual int GetSize();
	virtual DATA GetItem(int nIndex);
	DATA operator[](size_t nIndex)
	{
		if (nIndex >= m_aData.size())
		{
			cout<<"index out of range"<<endl;
			return -1;
		}
		return m_aData[nIndex];
	}

private:
	ConcreateIterater<DATA> *m_begin;
	ConcreateIterater<DATA> *m_end;
	vector<DATA> m_aData;
}; 

// 访问ConcreateAggregate 容器类的迭代器类 
template <class DATA> class ConcreateIterater : public Iterater 
{
public: 
	ConcreateIterater(Aggregate<DATA>* pAggregate)
		: m_pConcreateAggregate(pAggregate) 
		, m_nIndex(0) 
	{ 
	} 
	virtual ~ConcreateIterater(){} 
	ConcreateIterater& operator*()
	{
		return *this;
	}
	friend ostream &operator<<(ostream &os,const ConcreateIterater &itr)
	{
		os << itr.m_pConcreateAggregate->GetItem(itr.m_nIndex);

		return os;
	}
	friend istream &operator>>(istream &is,ConcreateIterater &itr)
	{
		int tmp = itr.m_pConcreateAggregate->GetItem(itr.m_nIndex);
		is >> tmp;
		return is;
	}
	virtual Iterater& operator++()//for ++itr
	{
		m_nIndex++;
		return *this;
	}
	virtual Iterater& operator++(int) //for itr++
	{
		ConcreateIterater *tmp = this;
		m_nIndex++;
		return *tmp;
	}
	bool operator==(ConcreateIterater itr)
	{
		return this->m_nIndex == itr.m_nIndex;
	}
	bool operator!=(ConcreateIterater itr)
	{
		return this->m_nIndex != itr.m_nIndex;
	}

	template <class DATA, int MAX_SIZE> friend class ConcreateAggregate;
private: 
	Aggregate<DATA>   *m_pConcreateAggregate; 
	int         m_nIndex; 
}; 

#endif 
