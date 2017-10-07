// CalculatorClient.cpp : 定义控制台应用程序的入口点。
//

#include <Ice/Ice.h>
#include <Ice/Application.h>
#include "Calculator.h"
#include <iostream>

using namespace std;
using namespace MCal;

//#define RC_List "/home/tester/Key_Checking/ATT_RC/RMT_B109P_EU_Set.txt"
//#define RC_Data "/home/tester/Key_Checking/ATT_RC/RMT_B109P_EU_Set.dat"

class MyApp : virtual public Ice::Application
{
public:
	virtual int run(int,char*[]);
};

int MyApp::run(int,char*[])
{
	Ice::ObjectPrx base = communicator()->stringToProxy("Cal:default -p 10000");
	CalPrx calculator = CalPrx::checkedCast(base);
	if(!calculator)
		throw "Invalid proxy!";
	cout<<calculator->add(1,2)<<endl;

	cout<<calculator->sub(56,12)<<endl;


	//Calc->Close();
	return 0;
}

#pragma comment(lib, "ice.lib")
#pragma comment(lib, "iceutil.lib")

int main1(int argc,char* argv[])
{
	MyApp app;

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
		Ice::ObjectPrx base = ic->stringToProxy("Cal:default -p 10000");  
		CalPrx printer = CalPrx::checkedCast(base);  
		if (!printer)  
		{  
			throw "Invalid proxy";  
		}  
		printer->add(5,7);  
	}  
	catch (const Ice::Exception& ex)  
	{  
		cerr << ex << endl;  
		status = 1;  
	}  
	catch (const char* msg)  
	{  
		cerr << msg << endl;  
		status = 1;  
	}  
	if (ic)  
		ic->destroy();

	system("pause");

	return status;  
}  
