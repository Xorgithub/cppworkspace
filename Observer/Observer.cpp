// Observer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "observer_impl.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Observer *p1 = new ConcreateObserver; //订阅者1
	Observer *p2 = new ConcreateObserver; //订阅者2

	Subject* p = new ConcreateSubject; //发布者
	p->Attach(p1); //添加订阅
	p->Attach(p2); 

	p->SetState(4); //状态改变
	p->Notify(); //发布通知

	p->Detach(p1); //删除订阅

	p->SetState(10); //状态改变
	p->Notify(); //发布通知

	delete p; 

	system("pause"); 

	return 0;
}

