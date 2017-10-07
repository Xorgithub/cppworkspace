

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
	// RAND_MAX ��rand()�����ɵ����ֵ 
	return rand()%(n==0? RAND_MAX : n);
}
int ( *pFun)(unsigned int) = &Rand;

// ����һ���������
void GetData(char *Buf)
{
	::EnterCriticalSection(&csGeneralData);
	random_shuffle(Data, Data+36, pFun);	// ���ƣ�VC6��׼֧�ֲ��ã�ֻ������д
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