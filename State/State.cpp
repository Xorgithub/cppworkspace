// State.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "state_impl.h"
#include <stdlib.h>

//允许一个对象在其内部状态改变时改变它的行为

int _tmain(int argc, _TCHAR* argv[])
{
	Context *pContext = new Context(new ConcreateStateA); 

	pContext->Request(); 

	pContext->Request(); 

	pContext->Request(); 

	delete pContext; 

	system("pause");

	return 0;
}

