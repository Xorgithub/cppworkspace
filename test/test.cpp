// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include<stdio.h>

int cmp(int a,int b)
{
	int x = 100;
	__asm
	{
		MOV eax,[a];
		CMP eax,[b];
		JLE min;
		MOV eax,b;
	}
min:
	__asm mov dword ptr[x],eax;
	return x;
}

int main(int argc,char **argv)
{
	char format[] = "%s %s\n";
	char hello[] = "hello";
	char world[] = "world";
	__asm
	{
		/*mov dword ptr [a],300;
		mov dword ptr [b],400;
		LEA EAX,a;
		mov EBX,[b];
		sub [EAX],EBX;
		INC [a];*/
		lea esi,world;
		push esi;
		lea esi,hello;
		push esi;
		lea esi,format;
		push esi;
		call dword ptr printf;
		//mov EAX,-1;
		//NEG EAX;
		//mov dword ptr [a],EAX;
		pop ebx;
		pop ebx;
		pop ebx;
	}

	int m = cmp(10000,2000);
	printf("\n%d\n",m);
	return 0;
}
