// TemplateMethod.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "template_method_impl.h"
#include <stdlib.h>

//定义一个操作中的法的骨架，而将一些步骤延迟到子类中。
//TemplateMethod 使得子类可以不改变一个算法的结构即可重定义该算法的某些特定步骤。 

int _tmain(int argc, _TCHAR* argv[])
{
	//抽象类定义模板，由子类特化模板
	AbstractClass* pConcreateClass = new ConcreateClass; 
	pConcreateClass->TemplateMethod(); 

	delete pConcreateClass; 

	system("pause");

	return 0;
}

