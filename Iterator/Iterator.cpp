// Iterator.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "iterator_impl.h"
#include "iterator_impl.cpp"
#include <stdlib.h>
#include <memory>
#include <iostream>
using namespace std;
//�ṩһ�ַ���˳�����һ���ۺ϶����и���Ԫ��,,���ֲ��豩¶�ö�����ڲ���ʾ.
typedef ConcreateAggregate<> dmVector;
typedef ConcreateAggregate<int,64> mVector;
typedef ConcreateIterater<int> mIterator;
int _tmain(int argc, _TCHAR* argv[])
{
	mVector vtdata;
	vtdata.push_back(1);
 	vtdata.push_back(2);
 	vtdata.push_back(3);
 	vtdata.push_back(4);
	for (mIterator itr  = vtdata.begin();itr != vtdata.end(); itr++) 
	{ 
		std::cout << *itr <<" "; 
	} 
	cout<<endl;
	for (int i=0;i<vtdata.GetSize();++i)
	{
		std::cout<<vtdata[i]<<" ";
	}
	cout<<endl;

	system("pause");

	return 0;
}

