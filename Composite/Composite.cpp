// Composite.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "composite_impl.h"
#include <stdlib.h>
#include <iostream>

//将对象组合成树形结构以表示部分-整体”的层次结构。
//Composite 使得用户对单个对象和组合对象的使用具有一致性。

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

