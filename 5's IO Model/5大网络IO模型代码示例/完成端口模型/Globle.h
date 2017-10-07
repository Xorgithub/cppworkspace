

#ifndef Globle_H
#define Globle_H

#include <iostream>
#include <string>
#include <algorithm>
#include <time.h>

using namespace std;

#define	BUFFER_SIZE		64
#define	PORT			4567

char Data[37] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

CRITICAL_SECTION csGeneralData;
CRITICAL_SECTION csShowMsg;


int Rand(unsigned int n)
{
	// RAND_MAX 是rand()能生成的最大值 
	return rand()%(n==0? RAND_MAX : n);
}
int ( *pFun)(unsigned int) = &Rand;

// 生成一个随机序列
void GetData(char *Buf)
{
	::EnterCriticalSection(&csGeneralData);
	random_shuffle(Data, Data+36, pFun);	// 郁闷，VC6标准支持不好，只能这样写
	memcpy(Buf, Data, 20);
	Buf[20] = '\0';
	::LeaveCriticalSection(&csGeneralData);
}


void ShowMsg(const char *pMsg)
{
	if(pMsg == NULL) return;
	EnterCriticalSection(&csShowMsg);
	cout << pMsg << endl;
	LeaveCriticalSection(&csShowMsg);
}
void ShowMsg(const string& sMsg)
{
	ShowMsg(sMsg.c_str());
}

#endif