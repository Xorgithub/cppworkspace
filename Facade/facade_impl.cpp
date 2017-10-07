
#include "stdafx.h"
#include "facade_impl.h" 
#include <iostream> 
using namespace std; 

void Subsystem1::Operation() 
{ 
	cout<<this<<" Subsystem1 operation"<<endl; 
} 

void Subsystem2::Operation() 
{ 
	cout<<this<<" Subsystem2 operation"<<endl; 
} 

Facade::Facade() 
{ 
	this->_subs1 = new Subsystem1(); 
	this->_subs2 = new Subsystem2(); 
}

Facade::~Facade() 
{ 
	delete _subs1; 
	delete _subs2; 
} 

void Facade::OperationWrapper() 
{ 
	this->_subs1->Operation(); 
	this->_subs2->Operation(); 
} 
