#pragma once

#ifndef SINGLETON_H 

#define SINGLETON_H 
#include <memory>
using namespace std;
class Singleton 
{ 
private: 
	Singleton(){}; 
public:
	// ��̬��Ա����,�ṩȫ�ַ��ʵĽӿ� 
	static Singleton* GetInstancePtr(); 
	static Singleton  GetInstance(); 
	void Test(); 
protected: 
	// ��̬��Ա����,�ṩȫ��Ωһ��һ��ʵ�� 
	static Singleton* m_pStatic; 
}; 

#endif 
