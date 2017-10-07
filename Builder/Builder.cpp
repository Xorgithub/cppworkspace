// Builder.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "builder_impl.h"
#include <stdlib.h>
#include <memory>
// 将一个复杂对象的构建与它的表示分离，使得同样的构建过程可以创建不同的表示
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	//构建过程一样，根据装载builder1和builder2创建不同表示
	auto_ptr<Director> pDirector1(new Director(new ConcreateBuilder1));
	pDirector1->Construct();
	
	auto_ptr<Director> pDirector2(new Director(new ConcreateBuilder2)); 
	pDirector2->Construct();  

	system( " pause " ); 

	return 0;
}

