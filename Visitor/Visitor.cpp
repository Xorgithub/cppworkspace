// Visitor.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "visitor_impl.h"
#include <stdlib.h>
//表示一个作用于某对象结构中的各元素的操作.它使你可以在不改变各元素的类的前提下定义作用于这些元素的新操作. 
//实现了"双分派"的概念，一个操作(Accept)既依赖于执行操作的具体类型(Element *)有依赖于操作传入参数的类型(Visitor *)
int _tmain(int argc, _TCHAR* argv[])
{
	Visitor *pVisitor1 = new ConcreateVisitorA(); 
	Visitor *pVisitor2 = new ConcreateVisitorB();

	Element *pElementA = new ConcreateElementA(); 
	pElementA->Accept(*pVisitor1); 
	pElementA->Accept(*pVisitor2); 

	Element *pElementB = new ConcreateElementB();
	pElementB->Accept(*pVisitor1); 
	pElementB->Accept(*pVisitor2); 

	delete pElementA; 
	delete pElementB; 
	delete pVisitor1;
	delete pVisitor2;

	system("pause");

	return 0;
}

