// memoryr.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <windows.h>
#include <process.h>
// int _tmain(int argc, _TCHAR* argv[])
// {
// 	return 0;
// }

#include <iostream>
using namespace std;

unsigned int __stdcall thread_func(void* lp)
{
	int nloop = 0;
	char buff[8];
	while (1)
	{
		nloop++;
		buff[nloop] = nloop;
		nloop += nloop*nloop;
		Sleep(10);
	}
}

int Fun()
{
	char *p = new char[16];
	memset(p, 0x00, 16*sizeof(char));

	p[0]  = 1; //�Ϸ���д
	p[15] = 2; //�Ϸ���д
	p[16] = 3; //Խ�磬delete�ɼ�����
	p[17] = 4; //Խ�磬delete�ɼ�����
	p[18] = 5; //Խ�磬delete�ɼ�����
	p[19] = 6; //Խ�磬delete�ɼ�����
	p[20] = 7; //Խ�磬Gflags�ɼ�����
	for (int i=0;i<50000;i++)
	{
		p[i] = i;
	}

	delete p;
	p = NULL;

	return 0;
}

int main()
{
	printf("put any char start test\n");
	getchar();
	int n = 4;
	int m = 5;
	int result = m*n+m+n;
	printf("start thread func\n");
	_beginthreadex(NULL,0,thread_func,NULL,0,NULL);
	Fun();

	int sub = result - n+m;
	printf("sub result:%d\n",sub);

	system("pause");

	return 0;
}