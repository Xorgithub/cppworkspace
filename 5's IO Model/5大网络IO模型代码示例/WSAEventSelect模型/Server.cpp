

#include <conio.h>
#include <stdio.h>
#include <winsock2.h>
//#include <mswsock.h>


#include "Globle.h"

#pragma comment(lib, "ws2_32.lib")


#define	MAX_CLIENT		FD_SETSIZE


SOCKET Listen;
HANDLE ThreadPool[FD_SETSIZE] = {NULL};
SOCKET Client[FD_SETSIZE] = {0};


SOCKET Socket[WSA_MAXIMUM_WAIT_EVENTS] = { 0 };
WSAEVENT Event[WSA_MAXIMUM_WAIT_EVENTS] = { NULL };
DWORD EventTotal = 0;

volatile BOOL bExit  = FALSE;
static int nThreadCout = 0;


void Init()
{
//	InitializeCriticalSection(&csFDRead);
	InitializeCriticalSection(&csGeneralData);
	InitializeCriticalSection(&csShowMsg);
	srand(time(0));
}

void BeforeExit()
{
//	DeleteCriticalSection(&csFDRead);
	DeleteCriticalSection(&csGeneralData);
	DeleteCriticalSection(&csShowMsg);
}

void CompressArray(DWORD Index)
{
	if(Index < 0 || Index > WSA_MAXIMUM_WAIT_EVENTS)
		return;
	Socket[Index] = Socket[EventTotal-1];
	Socket[EventTotal-1] = 0;
	WSAEVENT hEvent = Event[Index];
	Event[Index] = Event[EventTotal-1];
	::WSACloseEvent(hEvent);
	Event[EventTotal-1] = 0;
	EventTotal--;

}


DWORD WINAPI WorkerThread(LPVOID lpParam)  
{

	char Buf[BUFFER_SIZE] = {0};
	char Msg[1024] = "";

	ShowMsg("WorkerThread start...");

	DWORD dwCode;


	WSANETWORKEVENTS NetWorkEvent;
	DWORD Index;
	while(!bExit)
	{
		Index = ::WSAWaitForMultipleEvents(EventTotal, Event, FALSE, WSA_INFINITE, FALSE);
//		::WSAResetEvent(Event[Index - WSA_WAIT_EVENT_0]);
		::WSAEnumNetworkEvents(Socket[Index-WSA_WAIT_EVENT_0],
								Event[Index-WSA_WAIT_EVENT_0],
								&NetWorkEvent);

		sprintf(Msg, "Total clients:%d", EventTotal-1);
		ShowMsg(Msg);

		if(NetWorkEvent.lNetworkEvents & FD_ACCEPT)
		{
			if(NetWorkEvent.iErrorCode[FD_ACCEPT_BIT] != 0)
			{
				sprintf(Msg, "FD_ACCEPT failed with error %d",
						NetWorkEvent.iErrorCode[FD_ACCEPT_BIT]);
				ShowMsg(Msg);
				continue;
			}
			SOCKET Client = accept(Socket[Index - WSA_WAIT_EVENT_0], NULL, NULL);
			if(EventTotal > WSA_MAXIMUM_WAIT_EVENTS)
			{
				ShowMsg("Too many connections!");
				closesocket(Client);
				continue;
			}
			sprintf(Msg, "Socket %d connected...", Client);
			ShowMsg(Msg);

			WSAEVENT hEvent = ::WSACreateEvent();
			::WSAEventSelect(Client, hEvent, FD_READ | FD_WRITE | FD_CLOSE);
			Socket[EventTotal] = Client;
			Event[EventTotal] = hEvent;
			EventTotal++;
		}

		if(NetWorkEvent.lNetworkEvents & FD_READ)
		{
			if(NetWorkEvent.iErrorCode[FD_READ_BIT] != 0)
			{
				sprintf(Msg, "FD_READ failed with error %d",
						NetWorkEvent.iErrorCode[FD_READ_BIT]);
				ShowMsg(Msg);
				continue;
			}
			dwCode = recv(Socket[Index - WSA_WAIT_EVENT_0], Buf, BUFFER_SIZE, 0);
			if(dwCode == SOCKET_ERROR)
			{
				sprintf(Msg, "An error occured when reading data from socket %d, error code %d", 
						Socket[Index - WSA_WAIT_EVENT_0], ::WSAGetLastError());
				ShowMsg(Msg);
				continue;
			}
			Buf[dwCode] = '\0';
			sprintf(Msg, "Received data from socket %d: %s", 
					Socket[Index - WSA_WAIT_EVENT_0], Buf);
			ShowMsg(Msg);


			GetData(Buf);
			dwCode = send(Socket[Index - WSA_WAIT_EVENT_0], Buf, 21, 0);
			if(dwCode == SOCKET_ERROR)
			{
				sprintf(Msg, "An error occured when Sending data to socket %d, error code %d", 
						Socket[Index - WSA_WAIT_EVENT_0], ::WSAGetLastError());
				ShowMsg(Msg);
				continue;
			}
			sprintf(Msg, "Sended data to socket %d: %s",
					Socket[Index - WSA_WAIT_EVENT_0], Buf);
			ShowMsg(Msg);
		}

		/* 
		何时收到 FD_WRITE 消息
		1、有数据可以发出；   
		2、如果已完成了对一个非锁定连接调用的处理连接就会成功。

		FD_WRITE 被定义为当缓存由满->空时触发该事件(连接成功时会有一次，这是例外)   
		第一次FD_WRITE事件后，你就可以不停地发数据了。如果缓存的数据都被及时发送出
		去后就不会再有FD_WRITE事件。当发送返回WSAEWOULDBLOCK错误时，停止发送，等待
		FD_WRITE事件再次触发。
		*/
		if(NetWorkEvent.lNetworkEvents & FD_WRITE)	
		{
			if(NetWorkEvent.iErrorCode[FD_WRITE_BIT] != 0)
			{
				sprintf(Msg, "FD_WRITE failed with socke %d, error code %d",
						Socket[Index - WSA_WAIT_EVENT_0],
						NetWorkEvent.iErrorCode[FD_WRITE_BIT]);
				ShowMsg(Msg);
			}
			ShowMsg("--------------------------------------");
			
		}

		if(NetWorkEvent.lNetworkEvents & FD_CLOSE)
		{
			if(NetWorkEvent.iErrorCode[FD_CLOSE_BIT] != 0)
			{
				sprintf(Msg, "FD_CLOSE failed with socke %d, error code:%d", 
						Socket[Index - WSA_WAIT_EVENT_0],
						NetWorkEvent.iErrorCode[FD_CLOSE_BIT]);
				ShowMsg(Msg);
			//	continue;
			}
			sprintf(Msg, "Close socket %d", Socket[Index - WSA_WAIT_EVENT_0]);
			ShowMsg(Msg);

			closesocket(Socket[Index - WSA_WAIT_EVENT_0]);
			CompressArray(Index - WSA_WAIT_EVENT_0);
		}
		
	}



	ShowMsg("WorkerThread exit...");

	return 0;
}




int main()
{
	int i = 0;
	DWORD dwCode;
	WSADATA wsaData;
	HANDLE hThread = NULL;
	WSAEVENT NewEvent;

	Init();

	dwCode = WSAStartup(MAKEWORD(2,2), &wsaData);
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

	Listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(Listen == INVALID_SOCKET)
	{
		cout << "\nCan't create the socket:" << WSAGetLastError() << endl;
		goto EXIT;
	}
		
	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = ADDR_ANY;
	sockAddr.sin_port = htons(PORT);
	dwCode = bind(Listen, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	if(dwCode == SOCKET_ERROR)
	{
		::closesocket(Listen);
		cout << "\nCan't bind the socket:" << WSAGetLastError() << endl;
		goto EXIT;
	}

	NewEvent = ::WSACreateEvent();
	WSAEventSelect(Listen, NewEvent, FD_ACCEPT | FD_CLOSE);

	Socket[EventTotal] = Listen;
	Event[EventTotal] = NewEvent;
	EventTotal++;

	dwCode = listen(Listen, 20);
	if(dwCode == SOCKET_ERROR)
	{
		::closesocket(Listen);
		cout << "\nCan't listen:" << WSAGetLastError() << endl;
		goto EXIT;
	}

	
	hThread = ::CreateThread(NULL, NULL, WorkerThread, NULL, 0, NULL);	// CREATE_SUSPENDED
	ThreadPool[nThreadCout++] = hThread;
//	hThread = ::CreateThread(NULL, NULL, ReadThread, 0, 0, NULL);
//	ThreadPool[nThreadCout++] = hThread;
//	hThread = ::CreateThread(NULL, NULL, SendThread, 0, 0, NULL);
//	ThreadPool[nThreadCout++] = hThread;
	

	ShowMsg("Press 'q' to exit...\n");
	while(_getch() != 'q' && _getch() != 'Q')
	{
		ShowMsg("Press 'q' to exit...\n");
	}
	bExit = TRUE;
	::WSASetEvent(Event[0]);
	for(i = 0; i < EventTotal; i++)	// Listen已经包含在其中了
		closesocket(Socket[i]);
	
	for(i = 1; i < EventTotal; i++)
		WSACloseEvent(Event[i]);
//	::closesocket(Listen);

	::WaitForMultipleObjects(nThreadCout, ThreadPool, TRUE, INFINITE);
	for(i = 0; i < nThreadCout; i++)
		::CloseHandle(ThreadPool[i]);
	

EXIT:
	WSACleanup();
	BeforeExit();
	printf("\nPress any key to exit...");
	_getch();
	return 0;
}