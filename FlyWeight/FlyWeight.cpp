// FlyWeight.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "flyweight_impl.h"
#include <stdlib.h>

//���ù�������Ч��֧�ִ���ϸ���ȵĶ���

int _tmain(int argc, _TCHAR* argv[])
{
	FlyweightFactory flyweightfactory; 

	flyweightfactory.GetFlyweight("hello"); 
	flyweightfactory.GetFlyweight("world"); 
	flyweightfactory.GetFlyweight("hello"); 

	system("pause"); 
	return 0;
}

