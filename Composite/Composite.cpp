// Composite.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "composite_impl.h"
#include <stdlib.h>
#include <iostream>

//��������ϳ����νṹ�Ա�ʾ����-���塱�Ĳ�νṹ��
//Composite ʹ���û��Ե����������϶����ʹ�þ���һ���ԡ�

int _tmain(int argc, _TCHAR* argv[])
{
	Leaf *pLeaf1 = new Leaf(); 
	Leaf *pLeaf2 = new Leaf(); 

	Composite* pComposite = new Composite;
	std::cout<<"Add two Leaf"<<std::endl;
	pComposite->Add(pLeaf1); 
	pComposite->Add(pLeaf2); 

	pComposite->Operation(); 
	std::cout<<"GetChild(index = 2)"<<std::endl;
	pComposite->GetChild(2)->Operation(); 

	delete pComposite; 
	system("pause");

	return 0;
}

