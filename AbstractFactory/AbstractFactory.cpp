// AbstractFactory.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "abstractfactory_impl.h"
#include <stdlib.h>

//提供一个创建一系列相关或相互依赖对象的接口，而无需指定它们具体的类。
int _tmain(int argc, _TCHAR* argv[])
{
	// 生产产品A 的第一种实现 
	AbstractFactory *pFactory1 = new ConcreateFactory1; 
	AbstractProductA *pProductA = pFactory1->CreateProductA(); 

	// 生产产品B 的第二种实现 
	AbstractFactory *pFactory2 = new ConcreateFactory2; 
	AbstractProductB *pProductB = pFactory2->CreateProductB(); 

	delete pFactory1; 
	delete pProductA; 
	delete pFactory2; 
	delete pProductB; 

	system("pause"); 

	return 0;
}

