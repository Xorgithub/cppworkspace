// Singleton.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "singleton_impl.h"
#include <stdlib.h>
//��֤һ�������һ��ʵ�������ṩһ����������ȫ�ַ��ʵ㡣
int _tmain(int argc, _TCHAR* argv[])
{
	// ���ó�ʼ�������Ϳ��Է����� 
	Singleton::GetInstancePtr()->Test(); 
	Singleton::GetInstance().Test(); 

	system("pause");

	return 0;
}

