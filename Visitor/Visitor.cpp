// Visitor.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "visitor_impl.h"
#include <stdlib.h>
//��ʾһ��������ĳ����ṹ�еĸ�Ԫ�صĲ���.��ʹ������ڲ��ı��Ԫ�ص����ǰ���¶�����������ЩԪ�ص��²���. 
//ʵ����"˫����"�ĸ��һ������(Accept)��������ִ�в����ľ�������(Element *)�������ڲ����������������(Visitor *)
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

