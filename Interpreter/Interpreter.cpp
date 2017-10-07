// Interpreter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "interpreter_impl.h"
#include <stdlib.h>

//给定一个语言，定义它的文法的一种表示，并定义一个解释器，
//这个解释器使用该表示来解释语言中的句子。
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

