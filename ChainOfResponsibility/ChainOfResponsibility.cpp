// ChainOfResponsibility.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "chainofresponsibility_impl.h"
#include <stdlib.h>
//使多个对象都有机会处理请求,从而避免请求的发送者和接收者之间的耦合关系. 
//将这些对象连成一条链，并沿着这条链传递该请求,直到有一个对象处理它为止. 
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

