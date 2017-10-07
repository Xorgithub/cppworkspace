#pragma once

#ifndef FACTORY_H 

#define FACTORY_H 

class Product 
{ 
public: 
	Product(){} 
	virtual ~Product(){} 
}; 

class ConcreateProduct : public Product 
{ 
public: 
	ConcreateProduct(); 
	virtual ~ConcreateProduct(); 
}; 

class Factory 
{ 
public: 
	Factory(){} 
	virtual ~Factory(){} 
	Product* CreateProduct(); 

protected: 
	virtual Product* FactoryMethod() = 0; 
}; 

class ConcreateFactory : public Factory 
{ 
public: 
	ConcreateFactory(); 
	virtual ~ConcreateFactory(); 

protected: 
	virtual Product* FactoryMethod(); 
}; 

#endif 
