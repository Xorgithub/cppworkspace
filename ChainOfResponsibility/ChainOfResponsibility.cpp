// ChainOfResponsibility.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "chainofresponsibility_impl.h"
#include <stdlib.h>
//ʹ��������л��ᴦ������,�Ӷ���������ķ����ߺͽ�����֮�����Ϲ�ϵ. 
//����Щ��������һ���������������������ݸ�����,ֱ����һ����������Ϊֹ. 
int _tmain(int argc, _TCHAR* argv[])
{
	// p2::handlerequest-->p1::handlerequest
	Handler *p1 = new ConcreateHandler1(/*pNextHandler*/); 
	Handler *p2 = new ConcreateHandler2(p1); 

	p2->HandleRequest(); 

	delete p2; 
	system("pause");
	return 0;
}

