// AbstractFactory.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "abstractfactory_impl.h"
#include <stdlib.h>

//�ṩһ������һϵ����ػ��໥��������Ľӿڣ�������ָ�����Ǿ�����ࡣ
int _tmain(int argc, _TCHAR* argv[])
{
	// ������ƷA �ĵ�һ��ʵ�� 
	AbstractFactory *pFactory1 = new ConcreateFactory1; 
	AbstractProductA *pProductA = pFactory1->CreateProductA(); 

	// ������ƷB �ĵڶ���ʵ�� 
	AbstractFactory *pFactory2 = new ConcreateFactory2; 
	AbstractProductB *pProductB = pFactory2->CreateProductB(); 

	delete pFactory1; 
	delete pProductA; 
	delete pFactory2; 
	delete pProductB; 

	system("pause"); 

	return 0;
}

