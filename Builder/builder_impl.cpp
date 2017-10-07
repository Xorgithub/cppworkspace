#include "StdAfx.h"
#include "builder_impl.h"

#include <iostream> 
using namespace std;
void  ConcreateBuilder1::BuilderPartA() 
{ 
	std::cout<<" BuilderPartA by ConcreateBuilder1:"<<this<<endl; 
} 

void  ConcreateBuilder1::BuilderPartB() 
{ 
	std::cout<<" BuilderPartB by ConcreateBuilder1:"<<this<<endl; 
} 

void  ConcreateBuilder2::BuilderPartA() 
{ 
	std::cout<<" BuilderPartA by ConcreateBuilder2:"<<this<<endl; 
} 

void  ConcreateBuilder2::BuilderPartB() 
{ 
	std::cout<<" BuilderPartB by ConcreateBuilder2:"<<this<<endl; 
} 

Director::Director(Builder * pBuilder) 
: m_pBuilder(pBuilder) 
{
} 

Director:: ~Director() 
{
} 

// Construct ������ʾһ�������������������,��ͬ�Ĳ���֮���װ�䷽ʽ����һ�µ�, 
// ���ȹ���PartA �����PartB,ֻ�Ǹ��ݲ�ͬ�Ĺ����߻��в�ͬ�ı�ʾ 
void  Director::Construct() 
{ 
	m_pBuilder -> BuilderPartA(); 
	m_pBuilder -> BuilderPartB(); 
} 
