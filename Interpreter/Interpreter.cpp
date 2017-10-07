// Interpreter.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "interpreter_impl.h"
#include <stdlib.h>

//����һ�����ԣ����������ķ���һ�ֱ�ʾ��������һ����������
//���������ʹ�øñ�ʾ�����������еľ��ӡ�
int _tmain(int argc, _TCHAR* argv[])
{
	Context* c = new Context();
	AbstractExpression*  te  =  new TerminalExpression("hello");
	AbstractExpression*  nte  =  new NonterminalExpression(te,2);
	nte->Interpret(*c);

	delete nte;
	system("pause");

	return 0;
}

