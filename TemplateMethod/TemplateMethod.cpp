// TemplateMethod.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "template_method_impl.h"
#include <stdlib.h>

//����һ�������еķ��ĹǼܣ�����һЩ�����ӳٵ������С�
//TemplateMethod ʹ��������Բ��ı�һ���㷨�Ľṹ�����ض�����㷨��ĳЩ�ض����衣 

int _tmain(int argc, _TCHAR* argv[])
{
	//�����ඨ��ģ�壬�������ػ�ģ��
	AbstractClass* pConcreateClass = new ConcreateClass; 
	pConcreateClass->TemplateMethod(); 

	delete pConcreateClass; 

	system("pause");

	return 0;
}

