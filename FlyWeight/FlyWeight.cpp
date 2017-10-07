// FlyWeight.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "flyweight_impl.h"
#include <stdlib.h>

//运用共享技术有效地支持大量细粒度的对象。

int _tmain(int argc, _TCHAR* argv[])
{
	FlyweightFactory flyweightfactory; 

	flyweightfactory.GetFlyweight("hello"); 
	flyweightfactory.GetFlyweight("world"); 
	flyweightfactory.GetFlyweight("hello"); 

	system("pause"); 
	return 0;
}

