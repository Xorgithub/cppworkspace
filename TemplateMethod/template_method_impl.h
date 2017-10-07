#pragma once

// �������,���� �������� 
class AbstractClass 
{ 
public: 
	AbstractClass(){} 
	virtual ~AbstractClass(){} 

	// ��������ж����� �������� 
	void TemplateMethod(); 

protected: 
	// ���麯��,��������ʵ��֮ 
	virtual void PrimitiveOperation1() = 0; 
	virtual void PrimitiveOperation2() = 0; 
}; 

// �̳���AbstractClass,ʵ�� �� 
class ConcreateClass : public AbstractClass 
{ 
public: 
	ConcreateClass(){} 
	virtual ~ConcreateClass(){} 

protected: 
	virtual void PrimitiveOperation1(); 
	virtual void PrimitiveOperation2(); 
};