// Prototype.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "prototype_impl.h"

//��ԭ��ʵ��ָ��������������࣬����ͨ��������Щԭ�ʹ����µĶ���

int _tmain(int argc, _TCHAR* argv[])
{
	//����ԭ�Ͷ���
	resume *first_work = new resume;
	first_work->set_personal_info("jack","25","name","875964");
	first_work->set_work_experience("2012-2015","HP");
	first_work->print();
	//resume2 *first_work2 = new resume2;

	//�����¶��󲢿���ԭ�Ͷ��������
	resume *second_work = static_cast<resume *>(first_work->clone());
	second_work->set_work_experience("2015-2017","google");
	second_work->print();
	//resume2 *second_work2 = first_work2->clone();

	delete first_work;
	delete second_work;

	system("pause");

	return 0;
}

