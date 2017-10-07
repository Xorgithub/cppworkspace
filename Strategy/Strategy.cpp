// Strategy.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "strategy_impl.h"
#include <stdlib.h>

#include <memory>
using namespace std;
// 定义一系列的算法,把它们一个个封装起来, 并且使它们可相互替换.
// 本模式使得算法法可独立于使用它的客户而变化. 

int _tmain(int argc, _TCHAR* argv[])
{
	auto_ptr<Context> pContext(new Context(new ConcreateStrategyA())); 

	pContext->ContextInterface(); 
	pContext->ContextInterface(); 
	pContext->ContextInterface(); 

	system("pause");

	return 0;
}

