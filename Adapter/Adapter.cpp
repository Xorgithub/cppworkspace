// Adapter.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "adapter_impl.h"
#include <stdlib.h>
// ��һ����Ľӿ�ת���ɿͻ�ϣ��������һ���ӿڡ�Adapt ģʽʹ��ԭ�����ڽ� 
// �ڲ����ݶ�����һ��������Щ�����һ������ 
int _tmain(int argc, _TCHAR* argv[])
{
	Adaptee *pAdaptee = new Adaptee; //�¼���Ĳ����ݽӿ� 

	Target *pTarget = new Adapter(pAdaptee); //�Ѷ���ӿ�

	pTarget->Request(); 

	delete pTarget; 

	system("pause"); 

	return 0;
}

