// Facade.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include "facade_impl.h"
using namespace std;

// Ϊ��ϵͳ�е�һ��ӿ��ṩһ��һ�µĽ��棬 Facadeģʽ������һ���߲�ӿڣ������
// ��ʹ����һ��ϵͳ��������ʹ��

int _tmain(int argc, _TCHAR* argv[])
{
	Facade* f = new Facade(); 

	f->OperationWrapper();//����sub1��sub2��������

	delete f;
	system("pause");

	return 0;
}

