// Decorator.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>

#include "decorator_impl.h"

using namespace std;
//动态地给一个对象添加一些额外的职责。就增加功能来说，Decorator模式相比生成子类更为灵活
int _tmain(int argc, _TCHAR* argv[])
{
	// 初始化一个Component对象 
	Component* pComponent = new ConcreateComponent(); 
	// 采用这个Component对象去初始化一个Decorator对象, 
	cout<<"before decorator"<<endl;
	pComponent->Operation();
	cout<<endl;
	// 这样就可以为这个Component 对象动态添加职责 
	Decorator* pDecorator = new ConcreateDecorator(pComponent); 
	//Component* pDecorator = new ConcreateDecorator(pComponent); 
	cout<<"after decorator"<<endl;
	//对外表现为执行了component基本职责，并执行了decorator动态添加的职责
	pDecorator->Operation(); 
	
	delete pDecorator; 
	system("pause"); 

	return 0;
}

