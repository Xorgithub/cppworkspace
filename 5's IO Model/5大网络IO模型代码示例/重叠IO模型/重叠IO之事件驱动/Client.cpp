/*
		client.cpp  
*/
#include <Winsock2.h>
#include <stdio.h>
#include <conio.h>

#include <iostream>

#include "Globle.h"

#pragma comment(lib, "ws2_32.lib")

#define	MAX_THREAD		5

HANDLE ThreadPool[MAX_THREAD] = {NULL};

volatile BOOL bExit = FALSE;


void Init()
{
	InitializeCriticalSection(&csGeneralData);
	InitializeCriticalSection(&csShowMsg);
	srand(time(0));
}

void BeforeExit()
{
	DeleteCriticalSection(&csGeneralData);
	DeleteCriticalSection(&csShowMsg);
}

DWORD GetSocket(SOCKET &s)
{
	DWORD dwCode;
	char Msg[1024] = "";
	closesocket(s);
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //建立一个TCP/IP协议的套接字
	if(s == INVALID_SOCKET)
	{
		dwCode = WSAGetLastError();
		sprintf(Msg, "\nCan't create the socket:%d \n", dwCode);
		ShowMsg(Msg);
		return dwCode;
	}
	return 0;
}

DWORD DoConnect(SOCKET &s)
{
	DWORD dwCode;
//	char Msg[1024] = "";
	
	SOCKADDR_IN server;
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	dwCode = connect(s, (sockaddr*)&server, sizeof(server));
	return dwCode;
}
// 处理连接
DWORD WINAPI WorkerThread(LPVOID lpParam)
{
	char Msg[1024] = "";

	int iIndex = (int)lpParam;

	string sThreadName;
	sprintf(Msg, "WorkerThread %d ", iIndex);
	sThreadName = Msg;

	sprintf(Msg, "WorkerThread %d start...\n", iIndex);
	ShowMsg(Msg);
	

	char Buf[BUFFER_SIZE] = "";
	BOOL bConnect = FALSE;
	SOCKET s;
	DWORD dwCode;
	GetSocket(s);
	while(!bExit)
	{
		
		if(!bConnect)
		{
			while( (dwCode=DoConnect(s)) != 0 && !bExit)
			{
				dwCode = WSAGetLastError();
				sprintf(Msg, "can't connect to the server:%d \n", dwCode);
				::ShowMsg(sThreadName+Msg);

				if(dwCode != WSAECONNREFUSED && dwCode !=WSAENETUNREACH && dwCode != WSAETIMEDOUT)
				{
					GetSocket(s);
					sprintf(Msg, "create socket %d", s);
					ShowMsg(sThreadName + Msg);
				}
				Sleep(3000);
				ShowMsg(sThreadName + "connect to the server...");
			}
			if(dwCode == 0)
				bConnect = TRUE;
			if(bExit)
				break;
		}
		Sleep(2000);	// 延时2秒
		::GetData(Buf);
		dwCode = ::send(s, Buf, 20, 0);
		sprintf(Msg, "socket %d sended data to the server:%s", s, Buf);
		ShowMsg(sThreadName + Msg);

		if(dwCode == SOCKET_ERROR)
		{
			dwCode = ::WSAGetLastError();
			sprintf(Msg, "socket %d can't send data to the server:%d \n", s, dwCode);
			::ShowMsg(sThreadName + Msg);
		//	if(dwCode == WSAESHUTDOWN || dwCode == WSAECONNABORTED || dwCode == WSAECONNRESET
		//		|| dwCode == WSAENOTSOCK)
			{
				GetSocket(s);
				bConnect = FALSE;
				continue;
			}
		}

		dwCode = ::recv(s, Buf, BUFFER_SIZE, 0);
		if(dwCode == SOCKET_ERROR)
		{
			dwCode = ::WSAGetLastError();
			sprintf(Msg, "socket %d can't receive data from the server:%d \n", s, dwCode);
			::ShowMsg(sThreadName+Msg);
		//	if(dwCode == WSAESHUTDOWN || dwCode == WSAECONNABORTED || dwCode == WSAECONNRESET
		//		|| dwCode == WSAENOTSOCK)
			{
				GetSocket(s);
				bConnect = FALSE;
				continue;
			}
		}
		sprintf(Msg, "socket %d received data from the server:%s", s, Buf);
		ShowMsg(sThreadName+Msg);

		Sleep(500);
	}
	closesocket(s);

	sprintf(Msg, "WorkerThread %d exit...\n", iIndex);
	ShowMsg(Msg);
	return 0;
}

void main()
{
	int i;
	Init();

	WSADATA wsaData;
	DWORD dwCode = WSAStartup(MAKEWORD(2, 2), &wsaData);	//初始化WinSock
	if(dwCode != 0)
	{
		printf("\nCan't find find a usable WinSock DLL");
		goto EXIT;
	}
	if ( LOBYTE( wsaData.wVersion ) != 2 ||  HIBYTE( wsaData.wVersion ) != 2 )
	{
		printf("\nCan't find the socket version required.");
		goto EXIT;
    }


	for(i = 0; i < MAX_THREAD; i++)
	{
		HANDLE hThread = ::CreateThread(NULL, NULL, WorkerThread, (LPVOID)i, 0, NULL);
		ThreadPool[i] = hThread;
	}

	ShowMsg("Press 'q' to exit...\n");
	while(_getch() != 'q' && _getch() != 'Q')
	{
		ShowMsg("Press 'q' to exit...\n");
	}
	bExit = TRUE;

	::WaitForMultipleObjects(MAX_THREAD, ThreadPool, TRUE, INFINITE);
	for(i = 0; i < MAX_THREAD; i++)
		::CloseHandle(ThreadPool[i]);
	
EXIT:
	::WSACleanup();
	BeforeExit();
	printf("press any key to exit...");
	getch();
    return;
}
