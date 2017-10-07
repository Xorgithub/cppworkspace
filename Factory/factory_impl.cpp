#include "StdAfx.h"
#include "factory_impl.h"

#include <iostream> 

using namespace std; 

ConcreateProduct::ConcreateProduct() 
{ 
	std::cout << "construction of ConcreateProduct\n"; 
} 

ConcreateProduct::~ConcreateProduct() 
{ 
	std::cout << "destruction of ConcreateProduct\n"; 
} 

Product* Factory::CreateProduct() 
{ 
	std::cout << "an operation of product\n"; 
	Product* p = FactoryMethod(); //子类决定实例哪一个类
	return p;
} 

ConcreateFactory::ConcreateFactory() 
{ 
	std::cout << "construction of ConcreateFactory\n"; 
} 

ConcreateFactory::~ConcreateFactory() 
{ 
	std::cout << "destruction of ConcreateFactory\n"; 
} 

Product* ConcreateFactory::FactoryMethod() 
{ 
	return new ConcreateProduct(); 
} 
