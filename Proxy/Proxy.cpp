// Proxy.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "proxy_impl.h"
#include <stdlib.h>

//为其他对象提供一种代理以控制对这个对象的访问。
int _tmain(int argc, _TCHAR* argv[])
{
	Subject* pProxy = new Proxy(); 
	pProxy->Request(); 

	delete pProxy; 
	system("pause"); 

	return 0;
}

