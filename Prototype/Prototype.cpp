// Prototype.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "prototype_impl.h"

//用原型实例指定创建对象的种类，并且通过拷贝这些原型创建新的对象

int _tmain(int argc, _TCHAR* argv[])
{
	//创建原型对象
	resume *first_work = new resume;
	first_work->set_personal_info("jack","25","name","875964");
	first_work->set_work_experience("2012-2015","HP");
	first_work->print();
	//resume2 *first_work2 = new resume2;

	//创建新对象并拷贝原型对象的内容
	resume *second_work = static_cast<resume *>(first_work->clone());
	second_work->set_work_experience("2015-2017","google");
	second_work->print();
	//resume2 *second_work2 = first_work2->clone();

	delete first_work;
	delete second_work;

	system("pause");

	return 0;
}

