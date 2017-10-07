// Adapter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "adapter_impl.h"
#include <stdlib.h>
// 将一个类的接口转换成客户希望的另外一个接口。Adapt 模式使得原本由于接 
// 口不兼容而不能一起工作的那些类可以一起工作。 
int _tmain(int argc, _TCHAR* argv[])
{
	Adaptee *pAdaptee = new Adaptee; //新加入的不兼容接口 

	Target *pTarget = new Adapter(pAdaptee); //已定义接口

	pTarget->Request(); 

	delete pTarget; 

	system("pause"); 

	return 0;
}

