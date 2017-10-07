// Factory.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include "factory_impl.h"
#include <memory>
using namespace std;
//定义一个用于创建对象的接口，让子类决定实例化哪一个类。Factory  Method使一个类的实例化延迟到其子类。 
int _tmain(int argc, _TCHAR* argv[])
{
	//udp_factory,tcp_factory,rtsp_factory

	auto_ptr<Factory> fy(new ConcreateFactory()); 

	auto_ptr<Product> pt(fy->CreateProduct()); //决定实例化哪一个类，使一个类的实例化延迟到其子类

	system("pause"); 

	return 0;
}

