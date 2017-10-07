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

// Construct 函数表示一个对象的整个构建过程,不同的部分之间的装配方式都是一致的, 
// 首先构建PartA 其次是PartB,只是根据不同的构建者会有不同的表示 
void  Director::Construct() 
{ 
	m_pBuilder -> BuilderPartA(); 
	m_pBuilder -> BuilderPartB(); 
} 
