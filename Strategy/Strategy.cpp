// Strategy.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "strategy_impl.h"
#include <stdlib.h>

#include <memory>
using namespace std;
// ����һϵ�е��㷨,������һ������װ����, ����ʹ���ǿ��໥�滻.
// ��ģʽʹ���㷨���ɶ�����ʹ�����Ŀͻ����仯. 

int _tmain(int argc, _TCHAR* argv[])
{
	auto_ptr<Context> pContext(new Context(new ConcreateStrategyA())); 

	pContext->ContextInterface(); 
	pContext->ContextInterface(); 
	pContext->ContextInterface(); 

	system("pause");

	return 0;
}

