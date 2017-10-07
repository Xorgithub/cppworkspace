// Mediator.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "mediator_impl.h"
#include <stdlib.h>
#include <iostream>
using namespace std;
//��һ���н��������װһϵ�еĶ��󽻻����н���ʹ��������Ҫ��ʽ���໥���ã�
//�Ӷ�ʹ�������ɢ�����ҿ��Զ����ظı�����֮��Ľ�����
int _tmain(int argc, _TCHAR* argv[])
{
	// A<---->Mediator<---->B
	ConcreteMediator*  m  =  new ConcreteMediator();
	ConcreteColleageA*  c1  =  new ConcreteColleageA(m);
	ConcreteColleageB*  c2  =  new ConcreteColleageB(m);
	m->IntroColleage(c1,c2);//�н��߹���A��B
	c1->SetState("old");
	c2->SetState("old");
	c1->Aciton();//A�����н���ͨ�ŷ�����Bͨ��
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

