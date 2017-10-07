// IOCP_Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <conio.h>


#include "TcpServer.h"
#include "udpServer.h"
#include "WebServer.h"

int _tmain(int argc, _TCHAR* argv[])
{
 //   CTcpServer tcpServer;
	//tcpServer.InitServer("192.168.1.100",80);

	//tcpServer.StartServer();
 //   
	//getchar();
	//tcpServer.StopServer();

	//CUdpServer udpServer;
	//udpServer.InitServer("192.168.1.100",8080);

	//udpServer.StartServer();

	//getchar();
	//udpServer.StopServer();

	CWebServer webServer;
	webServer.InitServer("192.168.1.8",4567);

	webServer.StartServer();


	getche();
	webServer.StopServer();

	return 0;
}
