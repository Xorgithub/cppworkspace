// Proxy.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "proxy_impl.h"
#include <stdlib.h>

//Ϊ���������ṩһ�ִ����Կ��ƶ��������ķ��ʡ�
int _tmain(int argc, _TCHAR* argv[])
{
	Subject* pProxy = new Proxy(); 
	pProxy->Request(); 

	delete pProxy; 
	system("pause"); 

	return 0;
}

