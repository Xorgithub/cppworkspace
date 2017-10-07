// Mediator.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "mediator_impl.h"
#include <stdlib.h>
#include <iostream>
using namespace std;
//用一个中介对象来封装一系列的对象交互。中介者使各对象不需要显式地相互引用，
//从而使其耦合松散，而且可以独立地改变它们之间的交互。
int _tmain(int argc, _TCHAR* argv[])
{
	// A<---->Mediator<---->B
	ConcreteMediator*  m  =  new ConcreteMediator();
	ConcreteColleageA*  c1  =  new ConcreteColleageA(m);
	ConcreteColleageB*  c2  =  new ConcreteColleageB(m);
	m->IntroColleage(c1,c2);//中介者关联A和B
	c1->SetState("old");
	c2->SetState("old");
	c1->Aciton();//A调用中介者通信方法与B通信
	c2->Aciton();
	cout<<endl;
	c1->SetState("new");
	c1->Aciton();
	c2->Aciton();
	cout<<endl;
	c2->SetState("old");
	c2->Aciton();
	c1->Aciton();

	delete m;
	delete c1;
	delete c2;
	system("pause");

	return 0;
}

