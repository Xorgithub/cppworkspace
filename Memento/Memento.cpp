// Memento.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "memento_impl.h"
#include <stdlib.h>
//�ڲ��ƻ���װ�Ե�ǰ����,����һ��������ڲ�״̬,
//���ڸö���֮�Ᵽ�����״̬.�����Ժ�Ϳɽ��ö���ָ���ԭ�ȱ����״̬. 
int _tmain(int argc, _TCHAR* argv[])
{
	// ����һ��ԭ���� 
	Originator* pOriginator = new Originator("old state"); 
	pOriginator->PrintState(); 

	// ����һ������¼������ԭ������״̬ 
	Memento *pMemento = pOriginator->CreateMemento(); 

	// ����ԭ������״̬ 
	pOriginator->SetState("new state"); 

	pOriginator->PrintState(); 
	// ͨ������¼��ԭ������״̬��ԭ��֮ǰ��״̬ 
	pOriginator->RestoreState(pMemento); 

	pOriginator->PrintState(); 

	delete pOriginator; 
	delete pMemento;
	system("pause");

	return 0;
}

