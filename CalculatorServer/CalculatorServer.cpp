// ICE_Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include<Ice/Ice.h>
#include<Ice/Application.h>
#include<iostream>

#include "Caculator.h"

using namespace std;
using namespace MCal;

class CalI:public Cal
{
public:
	CalI()
	{
		num1d=0;
		num2d=0;
	}
	~CalI()
	{
	}
	virtual int add(int num1,int num2,const Ice::Current&);
	virtual int sub(int num1,int num2,const Ice::Current&);
private:
	int num1d;
	int num2d;
};

int CalI::add(int num1, int num2, const Ice::Current &)
{
	return (num1+num2);
}
int CalI::sub(int num1,int num2,const Ice::Current&)
{
	if (num1>num2)
	{
		return (num1-num2);
	}
	else
	{
		cout<<"there is some wrong!"<<endl;
		return 0;
	}
}

class CalApp:virtual public Ice::Application
{
public:
	virtual int run(int,char *[]);
};

int CalApp::run(int,char *[])
{
	//shutdownOnInterrupt();
	Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapterWithEndpoints(
		"Caladapter","default -p 10000");
	Ice::ObjectPtr object = new CalI();
	adapter->add(object,communicator()->stringToIdentity("Cal"));
	adapter->activate();
	communicator()->waitForShutdown();

	if(interrupted())
		cerr << appName() << ":recv signal,shutting down" << endl;

	return 0;
}

#pragma comment(lib, "iced.lib")
#pragma comment(lib, "iceutild.lib")

int main1(int argc,char* argv[])
{
	CalApp app;
	app.main(argc,argv);

	system("pause");

	return 0;
}

int main(int argc, char* argv[])  
{  
	int status = 0;  
	Ice::CommunicatorPtr ic;  
	try  
	{  
		ic = Ice::initialize(argc, argv);  
		Ice::ObjectAdapterPtr adapter = ic->createObjectAdapterWithEndpoints("CalAdatper", "default -p 10000");  
		Ice::ObjectPtr object = new CalI(); 
		adapter->add(object, ic->stringToIdentity("Cal")); 
		adapter->activate();  
		ic->waitForShutdown();  
	}  
	catch (const Ice::Exception& e)  
	{  
		cerr << e << endl;  
		status = 1;  
	}  
	catch (const char* msg)  
	{  
		cerr << msg << endl;  
		status = 1;  
	}  
	catch (std::exception& ex)  
	{  
		cerr << ex.what() << endl;  
		status = 1;  
	}  
	if (ic)  
	{  
		ic->destroy();  
	}
	system("pause");

	return status;  
}
