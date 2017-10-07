// Bridge.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "bridge_impl.h"
#include <stdlib.h>
#include <stdio.h>

//将抽象部分与它的实现部分分离，使它们都可以独立地变化

int _tmain(int argc, _TCHAR* argv[])
{
	/************************************************************************/
	/* media_link::create_link(link_type) （提供抽象接口）-->                 */
	/* --> media_link_impl::create_link(new tcp,new udp,new rtsp) （传入不同对象指针）*/
	/* --> tcp_link_impl::create_link,udp_link_impl::create_link,---        */
	/* --- rtsp_link_impl::create_link  （不同具体实现）                        */
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

