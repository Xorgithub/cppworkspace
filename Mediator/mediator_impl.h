#pragma once

//#ifndef _COLLEAGE_H_
//#define _COLLEAGE_H_
#include <string>

using namespace std;
class Mediator;
class Colleage
{
public:
	virtual ~Colleage();
	virtual void Aciton() = 0;
	virtual void SetState(const string& sdt) = 0;
	virtual string GetState() = 0;
protected:
	Colleage();
	Colleage(Mediator* mdt);
	Mediator* _mdt;
private:
};
class ConcreteColleageA:public Colleage
{
public:
	ConcreteColleageA();
	ConcreteColleageA(Mediator* mdt);
	~ConcreteColleageA();
	void Aciton();
	void SetState(const string& sdt);
	string GetState();
protected:
private:
	string _sdt;
};
class ConcreteColleageB:public Colleage
{
public:
	ConcreteColleageB();
	ConcreteColleageB(Mediator* mdt);
	~ConcreteColleageB();
	void Aciton();
	void SetState(const string& sdt);
	string GetState();
protected:
private:
	string _sdt;
};
//#endif //~_COLLEAGE_H_

//Mediator.h
//#ifndef _MEDIATOR_H_
//#define _MEDIATOR_H_
class Colleage;
class Mediator
{
public:
	virtual ~Mediator();
	virtual void DoActionFromAtoB() = 0;
	virtual void DoActionFromBtoA() = 0;
protected:
	Mediator();
private:
};
class ConcreteMediator:public Mediator
{
public:
	ConcreteMediator();
	ConcreteMediator(Colleage* clgA,Colleage* clgB);
	~ConcreteMediator();
	void  SetConcreteColleageA(Colleage* clgA);
	void  SetConcreteColleageB(Colleage* clgB);
	Colleage* GetConcreteColleageA();
	Colleage* GetConcreteColleageB();
	void  IntroColleage(Colleage* clgA,Colleage* clgB);
	void DoActionFromAtoB();
	void DoActionFromBtoA();
protected:
private:
	Colleage* _clgA;
	Colleage* _clgB;
};
//#endif //~_MEDIATOR_H