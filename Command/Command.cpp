// Command.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "command_impl.h"
#include <stdlib.h>

//��һ�������װΪһ������,�Ӷ�ʹ����ò�ͬ������Կͻ����в�����;
//�������Ŷӻ��¼������־,�Լ�֧�ֿɳ����Ĳ���. 
int _tmain(int argc, _TCHAR* argv[])
{
	Receiver* pReceiver = new Receiver();//�ص��������� 
	Command* pCommand   = new ConcreateComand(pReceiver);//�ص����� 

	//invoker����֪������Ľ�����
	Invoker* pInvoker   = new Invoker(pCommand); //ע��ص�

	pInvoker->Invoke(); //�ȴ���ע��ص����ڻص��д����������

	delete pInvoker; 

	system("pause"); 
	return 0;
}

