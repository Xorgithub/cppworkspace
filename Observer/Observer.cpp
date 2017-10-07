// Observer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "observer_impl.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Observer *p1 = new ConcreateObserver; //������1
	Observer *p2 = new ConcreateObserver; //������2

	Subject* p = new ConcreateSubject; //������
	p->Attach(p1); //��Ӷ���
	p->Attach(p2); 

	p->SetState(4); //״̬�ı�
	p->Notify(); //����֪ͨ

	p->Detach(p1); //ɾ������

	p->SetState(10); //״̬�ı�
	p->Notify(); //����֪ͨ

	delete p; 

	system("pause"); 

	return 0;
}

