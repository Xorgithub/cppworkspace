// Singleton.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "singleton_impl.h"
#include <stdlib.h>
//保证一个类仅有一个实例，并提供一个访问它的全局访问点。
int _tmain(int argc, _TCHAR* argv[])
{
	// 不用初始化类对象就可以访问了 
	Singleton::GetInstancePtr()->Test(); 
	Singleton::GetInstance().Test(); 

	system("pause");

	return 0;
}

