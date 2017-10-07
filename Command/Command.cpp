// Command.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "command_impl.h"
#include <stdlib.h>

//将一个请求封装为一个对象,从而使你可用不同的请求对客户进行参数化;
//对请求排队或记录请求日志,以及支持可撤消的操作. 
int _tmain(int argc, _TCHAR* argv[])
{
	Receiver* pReceiver = new Receiver();//回调处理事务 
	Command* pCommand   = new ConcreateComand(pReceiver);//回调函数 

	//invoker并不知道命令的接收者
	Invoker* pInvoker   = new Invoker(pCommand); //注册回调

	pInvoker->Invoke(); //先处理注册回调，在回调中处理具体事务

	delete pInvoker; 

	system("pause"); 
	return 0;
}

