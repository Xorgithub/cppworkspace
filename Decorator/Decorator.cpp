// Decorator.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>

#include "decorator_impl.h"

using namespace std;
//��̬�ظ�һ���������һЩ�����ְ�𡣾����ӹ�����˵��Decoratorģʽ������������Ϊ���
int _tmain(int argc, _TCHAR* argv[])
{
	// ��ʼ��һ��Component���� 
	Component* pComponent = new ConcreateComponent(); 
	// �������Component����ȥ��ʼ��һ��Decorator����, 
	cout<<"before decorator"<<endl;
	pComponent->Operation();
	cout<<endl;
	// �����Ϳ���Ϊ���Component ����̬���ְ�� 
	Decorator* pDecorator = new ConcreateDecorator(pComponent); 
	//Component* pDecorator = new ConcreateDecorator(pComponent); 
	cout<<"after decorator"<<endl;
	//�������Ϊִ����component����ְ�𣬲�ִ����decorator��̬��ӵ�ְ��
	pDecorator->Operation(); 
	
	delete pDecorator; 
	system("pause"); 

	return 0;
}

