#pragma once

#include <string>
using namespace std;

class Context //表述内容
{
public:
	Context();
	~Context();
protected:
private:
};

class AbstractExpression //抽象表达式
{
public:
	virtual ~AbstractExpression();
	virtual void Interpret(const Context& c);
protected:
	AbstractExpression();
private:
};
class  TerminalExpression:public //终止符表达式
	AbstractExpression
{
public:
	TerminalExpression(const  string&statement);
	~TerminalExpression();
	void Interpret(const Context& c);
protected:
private:
	string _statement;
};
class  NonterminalExpression:public //非终止符表达式
	AbstractExpression
{
public:
	NonterminalExpression(AbstractExpression* expression,int times);
	~NonterminalExpression();
	void Interpret(const Context& c);
protected:
private:
	AbstractExpression* _expression;
	int _times;
};
