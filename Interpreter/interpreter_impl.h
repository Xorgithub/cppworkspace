#pragma once

#include <string>
using namespace std;

class Context //��������
{
public:
	Context();
	~Context();
protected:
private:
};

class AbstractExpression //������ʽ
{
public:
	virtual ~AbstractExpression();
	virtual void Interpret(const Context& c);
protected:
	AbstractExpression();
private:
};
class  TerminalExpression:public //��ֹ�����ʽ
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
class  NonterminalExpression:public //����ֹ�����ʽ
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
