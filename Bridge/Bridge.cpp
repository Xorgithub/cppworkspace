// Bridge.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "bridge_impl.h"
#include <stdlib.h>
#include <stdio.h>

//�����󲿷�������ʵ�ֲ��ַ��룬ʹ���Ƕ����Զ����ر仯

int _tmain(int argc, _TCHAR* argv[])
{
	/************************************************************************/
	/* media_link::create_link(link_type) ���ṩ����ӿڣ�-->                 */
	/* --> media_link_impl::create_link(new tcp,new udp,new rtsp) �����벻ͬ����ָ�룩*/
	/* --> tcp_link_impl::create_link,udp_link_impl::create_link,---        */
	/* --- rtsp_link_impl::create_link  ����ͬ����ʵ�֣�                        */
	/************************************************************************/

	ConcreateImplementorA *pImplA = new ConcreateImplementorA(); 
	Abstraction *pAbstraction1 = new Abstraction(pImplA); 
	pAbstraction1->Operation();
	delete pAbstraction1; 

	ConcreateImplementorB *pImplB = new ConcreateImplementorB(); 
	Abstraction *pAbstraction2 = new Abstraction(pImplB); 
	pAbstraction2->Operation(); 
	delete pAbstraction2; 

	system("pause"); 

	return 0;
}

