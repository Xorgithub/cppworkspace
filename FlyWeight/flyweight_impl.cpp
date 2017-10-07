#include "StdAfx.h"
#include "flyweight_impl.h"

#include <iostream> 

inline STATE Flyweight::GetIntrinsicState() 
{ 
	return m_State; 
} 

FlyweightFactory::~FlyweightFactory() 
{ 
	for (std::list<Flyweight*>::iterator iter = m_listFlyweight.begin();
		iter != m_listFlyweight.end(); ++iter) 
	{ 
		delete (*iter);
	} 
	m_listFlyweight.clear(); 
} 

Flyweight* FlyweightFactory::GetFlyweight(const STATE& key) 
{
	for (std::list<Flyweight*>::iterator iter = m_listFlyweight.begin();
		iter != m_listFlyweight.end(); ++iter) 
	{ 
		if ((*iter)->GetIntrinsicState() == key) 
		{ 
			std::cout << "The Flyweight:" << key << " already exits"<< std::endl; 
			return (*iter); 
		} 
	} 

	std::cout << "Creating a new Flyweight:" << key << std::endl; 
	Flyweight* flyweight = new ConcreateFlyweight(key); 
	m_listFlyweight.push_back(flyweight); 

	return flyweight;
} 

void ConcreateFlyweight::Operation(STATE& ExtrinsicState) 
{ 
	std::cout<<ExtrinsicState<<std::endl;
} 
