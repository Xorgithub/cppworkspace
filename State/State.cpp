// State.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "state_impl.h"
#include <stdlib.h>

//����һ�����������ڲ�״̬�ı�ʱ�ı�������Ϊ

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

