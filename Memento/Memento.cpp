// Memento.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "memento_impl.h"
#include <stdlib.h>
//在不破坏封装性的前提下,捕获一个对象的内部状态,
//并在该对象之外保存这个状态.这样以后就可将该对象恢复到原先保存的状态. 
int _tmain(int argc, _TCHAR* argv[])
{
	// 创建一个原发器 
	Originator* pOriginator = new Originator("old state"); 
	pOriginator->PrintState(); 

	// 创建一个备忘录存放这个原发器的状态 
	Memento *pMemento = pOriginator->CreateMemento(); 

	// 更改原发器的状态 
	pOriginator->SetState("new state"); 

	pOriginator->PrintState(); 
	// 通过备忘录把原发器的状态还原到之前的状态 
	pOriginator->RestoreState(pMemento); 

	pOriginator->PrintState(); 

	delete pOriginator; 
	delete pMemento;
	system("pause");

	return 0;
}

