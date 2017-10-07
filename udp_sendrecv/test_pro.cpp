// test_pro.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <map>
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	map<int,string> map_sort;
	map_sort.insert(make_pair(75,"jack10"));
	map_sort.insert(make_pair(85,"jack3"));
	map_sort.insert(make_pair(65,"jack7"));
	map_sort.insert(make_pair(175,"jack9"));
	map_sort.insert(make_pair(725,"jack2"));
	map_sort.insert(make_pair(15,"jack5"));
	map_sort.insert(make_pair(751,"jack4"));
	map_sort.insert(make_pair(715,"jack1"));
	map_sort.insert(make_pair(51,"jack12"));

	map<int,string>::iterator itr = map_sort.begin();
	for (;itr!=map_sort.end();++itr)
	{
		printf("no:%d name:%s\n",itr->first,itr->second.c_str());
	}

	system("pause");
	return 0;
}

