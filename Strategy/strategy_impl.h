#pragma once

#ifndef STRATEGY_H 

#define STRATEGY_H 

class Strategy; 

class Context 
{ 
public: 
	Context(Strategy *pStrategy); 
	~Context(); 
	void ContextInterface(); 

private: 
	Strategy* m_pStrategy; 
}; 

class Strategy 
{ 
public: 
	virtual ~Strategy(){} 
	virtual void AlgorithmInterface() = 0;
	virtual Strategy* SwitchAlgorithm() = 0;
}; 

class ConcreateStrategyA : public Strategy 
{ 
public: 
	virtual ~ConcreateStrategyA(){} 
	virtual void AlgorithmInterface(); 
	virtual Strategy* SwitchAlgorithm();
}; 

class ConcreateStrategyB : public Strategy 
{ 
public: 
	virtual ~ConcreateStrategyB(){} 
	virtual void AlgorithmInterface();
	virtual Strategy* SwitchAlgorithm();
}; 
#endif 