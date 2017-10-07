// test_space.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <string>
#include <map>
#include <windows.h>
#include <process.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <sys/stat.h>

using namespace std;

typedef unsigned short  uint16_t;


void DeleteLogFile(const char *pstrFName, DWORD nMaxSize)
{
	long nFileSize = 0;
	int fh = 0;
	errno_t err = _sopen_s( &fh,pstrFName, _S_IWRITE, _S_IWRITE, _S_IWRITE);

	if(fh != -1)
	{
		nFileSize = _filelength(fh);
		if(nFileSize<0)
			nFileSize = 0;
		_close( fh );
	}

	if((DWORD)nFileSize>=nMaxSize)
	{
		DeleteFile(pstrFName);
	}
}

void WriteLog(LPCSTR pStr, LPCSTR pstrIndex)
{	
	FILE *fp = NULL;
	errno_t err = fopen_s(&fp,pstrIndex, "a"); 
	if (fp)
	{
		fprintf(fp,"[%d] %s\n", GetTickCount()/1000, pStr);		
		fclose(fp);		
	}
}


map<int,string> map_frame_container;

typedef int (__stdcall *pfn)(int i,char **,int **);

#define MP_SN_JUDGE_CONDITION 32767

class sn_cmp
{
public:
	bool operator()(uint16_t sn1,uint16_t sn2)
	{
		if (sn1<sn2 && (sn2-sn1)<MP_SN_JUDGE_CONDITION)
		{
			return true;
		}
		else if (sn1>sn2 && (sn1-sn2)>MP_SN_JUDGE_CONDITION)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

map<uint16_t,string,sn_cmp> map_sn;

multimap<string,string> phone_book;

int __stdcall func(int i,char **dst,int **prm)
{
	phone_book.insert(make_pair("Jack", "1111"));
	phone_book.insert(make_pair("Jack", "2222"));
	phone_book.insert(make_pair("Peter", "4444"));
	phone_book.insert(make_pair("Rose", "1111"));
	phone_book.insert(make_pair("Rose", "2222"));
	phone_book.insert(make_pair("Rose", "3333"));
	phone_book.insert(make_pair("Tim", "5555"));
	std::cout<<"count key=Jack is "<<phone_book.count("Jack")<<std::endl;

	typedef multimap<string, string>::const_iterator MCITR;
	typedef pair<MCITR, MCITR> Range;
	Range range = phone_book.equal_range("rose");
	for (MCITR itr=range.first;itr!=range.second;++itr)
	{
		std::cout<<itr->second<<std::endl;
	}
	if (int i=1)
	{
		std::cout<<"out i "<<i<<endl;
	}
	if (int i = 0 ? 1:2)
	{
		std::cout<<"put out i= "<<i<<endl;
	}
	//count header-lower but not tail-upper
	MCITR cit = phone_book.upper_bound("rose");//find first upper than k
	if (cit != phone_book.end())
	{
		std::cout<<cit->second<<endl;
	}
	MCITR cit2 = phone_book.lower_bound("rose");//find first ref to key
	if (cit2 != phone_book.end())
	{
		std::cout<<cit2->second<<std::endl;
	}
	map_sn.insert(make_pair(65534,"hello1"));
	map_sn.insert(make_pair(1,"hello4"));
	map_sn.insert(make_pair(65531,"hello2"));
	map_sn.insert(make_pair(65535,"hello3"));
	map_sn.insert(make_pair(3,"hello4"));
	map_sn.insert(make_pair(65532,"hello5"));
	map_sn.insert(make_pair(0,"hello"));

	char *buff = new char[1024];
	char *tmp = "hello world";

	memcpy(buff,tmp,strlen(tmp)+1);

	*dst = buff;

	return 0;
}

char *tmp_buff = NULL;
unsigned int __stdcall threadfunc1(void *)
{
	while (static int i=3)
	{
		map_frame_container.insert(make_pair(i++,"helllo"));
		std::cout<<"T1 insert i="<<i<<endl;
	}
	return 0;
};
unsigned int __stdcall threadfunc2(void *)
{
	while (1)
	{
		for (map<int,string>::iterator itr = map_frame_container.begin();
			itr!=map_frame_container.end();++itr)
		{
			std::cout<<"T2 key= "<<itr->first<<endl;
		}
	}
	return 0;
};
unsigned int __stdcall threadfunc3(void *)
{
	while (static int i=3)
	{
		map<int,string>::iterator itr = map_frame_container.find(i++);
		if(itr != map_frame_container.end())
		{
			map_frame_container.erase(itr);
			std::cout<<"T3 erase i= "<<i<<endl;
		}
	}
	return 0;
};

int _tmain(int argc, _TCHAR* argv[])
{
	::_beginthreadex(NULL,0,&threadfunc1,NULL,0,NULL);

	::_beginthreadex(NULL,0,&threadfunc2,NULL,0,NULL);

	Sleep(3000);
	::_beginthreadex(NULL,0,&threadfunc3,NULL,0,NULL);

	system("pause");
	return 0;
}

