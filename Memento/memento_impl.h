#pragma once

#ifndef MEMENTO_H 

#define MEMENTO_H 

#include <string> 

typedef std::string State; 

class Memento; 

class Originator 
{ 
public: 
	Originator(const State& rState); 
	Originator(); 
	~Originator(); 
	Memento*  CreateMemento(); 
	void      SetMemento(Memento* pMemento); 
	State     GetState(); 
	void      SetState(const State& rState); 
	void      RestoreState(Memento* pMemento); 
	void      PrintState(); 

private: 
	State      m_State; 
}; 

// 把Memento 的接口函数都设置为私有的,而Originator 是它的友元, 
// 这样保证了只有Originator 可以对其访问 
class Memento 
{ 
private: 
	friend class Originator; 
	Memento(const State& rState); 
	void   SetState(const State& rState); 
	State  GetState(); 
	State  m_State; 
}; 

#endif 
