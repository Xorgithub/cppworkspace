// Facade.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include "facade_impl.h"
using namespace std;

// 为子系统中的一组接口提供一个一致的界面， Facade模式定义了一个高层接口，这个接
// 口使得这一子系统更加容易使用

int _tmain(int argc, _TCHAR* argv[])
{
	Facade* f = new Facade(); 

	f->OperationWrapper();//包含sub1，sub2两个操作

	delete f;
	system("pause");

	return 0;
}

