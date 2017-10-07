#pragma once

#ifndef OBSERVER_H 

#define OBSERVER_H 
#include <list> 

typedef int STATE; 

class Observer; 

//发布者
// Subject 抽象基类,只需要知道Observer 基类的声明就可以了 
class Subject 
{ 
public: 
	Subject() : m_nSubjectState(-1){} 
	virtual ~Subject(); 
	void Notify();                   // 通知对象改变状态 
	void Attach(Observer *pObserver);       // 新增对象 
	void Detach(Observer *pObserver);        // 删除对象 

	// 虚函数,提供默认的实现,派生类可以自己实现来覆 基类的实现 
	virtual void  SetState(STATE nState);    // 设置状态 
	virtual STATE   GetState();      // 得到状态 

protected: 
	STATE m_nSubjectState;                 // 模拟保存Subject 状态的变量 
	std::list<Observer*>    m_ListObserver;    // 保存Observer 指针的链表 
}; 

// ConcreateSubject 类,派生在Subject 类 
class ConcreateSubject : public Subject 
{ 
public: 
	ConcreateSubject() : Subject(){} 
	virtual ~ConcreateSubject(){} 

	// 派生类自己实现来覆 基类的实现 
	virtual void   SetState(STATE nState);     // 设置状态 
	virtual STATE    GetState();       // 得到状态 
}; 

//订阅者
// Observer 抽象基类 
class Observer 
{ 
public: 
	Observer() : m_nObserverState(-1){} 
	virtual ~Observer(){} 

	// 纯虚函数,各个派生类可能有不同的实现 
	// 通知Observer 状态发生了变化 
	virtual void Update(Subject* pSubject) = 0; 

protected: 
	STATE m_nObserverState; // 模拟保存Observer 状态的变量 
}; 
// ConcreateObserver 类派生自Observer 
class ConcreateObserver : public Observer 
{ 
public: 
	ConcreateObserver() : Observer(){} 
	virtual ~ConcreateObserver(){} 

	// 虚函数,实现基类提供的接口 
	virtual void Update(Subject* pSubject); 
}; 

#endif 