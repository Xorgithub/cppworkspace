// Factory.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdlib.h>
#include "factory_impl.h"
#include <memory>
using namespace std;
//����һ�����ڴ�������Ľӿڣ����������ʵ������һ���ࡣFactory  Methodʹһ�����ʵ�����ӳٵ������ࡣ 
int _tmain(int argc, _TCHAR* argv[])
{
	//udp_factory,tcp_factory,rtsp_factory

	auto_ptr<Factory> fy(new ConcreateFactory()); 

	auto_ptr<Product> pt(fy->CreateProduct()); //����ʵ������һ���࣬ʹһ�����ʵ�����ӳٵ�������

	system("pause"); 

	return 0;
}

