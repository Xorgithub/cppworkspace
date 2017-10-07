// Builder.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "builder_impl.h"
#include <stdlib.h>
#include <memory>
// ��һ�����Ӷ���Ĺ��������ı�ʾ���룬ʹ��ͬ���Ĺ������̿��Դ�����ͬ�ı�ʾ
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	//��������һ��������װ��builder1��builder2������ͬ��ʾ
	auto_ptr<Director> pDirector1(new Director(new ConcreateBuilder1));
	pDirector1->Construct();
	
	auto_ptr<Director> pDirector2(new Director(new ConcreateBuilder2)); 
	pDirector2->Construct();  

	system( " pause " ); 

	return 0;
}

