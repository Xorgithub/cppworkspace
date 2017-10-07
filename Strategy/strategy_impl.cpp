#include "StdAfx.h"
#include "strategy_impl.h"

#include <iostream> 
#include <memory>
using namespace std;
Context::Context(Strategy *pStrategy) 
: m_pStrategy(pStrategy) 
{ 
} 

Context::~Context() 
{ 
	delete m_pStrategy; 
	m_pStrategy = NULL; 
} 

void Context::ContextInterface() 
{ 
	if (NULL != m_pStrategy) 
	{ 
		m_pStrategy->AlgorithmInterface();

		Strategy* pTmp = m_pStrategy->SwitchAlgorithm();
		delete m_pStrategy;
		m_pStrategy = pTmp;
	} 
} 

void ConcreateStrategyA::AlgorithmInterface() 
{ 
	std::cout <<this<< " AlgorithmInterface Implemented by ConcreateStrategyA\n";
}

Strategy *ConcreateStrategyA::SwitchAlgorithm()
{
	return new ConcreateStrategyB;
}

void ConcreateStrategyB::AlgorithmInterface() 
{ 
	std::cout <<this<< " AlgorithmInterface Implemented by ConcreateStrategyB\n";
} 

Strategy* ConcreateStrategyB::SwitchAlgorithm() 
{ 
	return new ConcreateStrategyA;
} 