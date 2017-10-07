

#include <conio.h>
#include <stdio.h>
#include <winsock2.h>
//#include <mswsock.h>


#include "Globle.h"

#pragma comment(lib, "ws2_32.lib")


//#define	MAX_CLIENT		5

#define NEXT_IO_READ		0
#define NEXT_IO_SEND		1

SOCKET Listen;
HANDLE ThreadPool[WSA_MAXIMUM_WAIT_EVENTS] = {NULL};

CRITICAL_SECTION csAccept;


volatile DWORD EventTotal = 0;
volatile DWORD TotalClients = 0;			// 

volatile DWORD dwLeftConnect = 0;
SOCKET Client[WSA_MAXIMUM_WAIT_EVENTS];		// 临时存放连接的客户端队列, 取出以后dwLeftConnect减一


WSAEVENT Event[WSA_MAXIMUM_WAIT_EVENTS];

volatile BOOL bExit  = FALSE;
static int nThreadCout = 0;


typedef struct		// 第一个成员必须是OverLapped结构
{
	OVERLAPPED Overlapped;
	WSABUF DataBuf;
	CHAR Buffer[BUFFER_SIZE];
	DWORD dwOpType;
	SOCKET Socket;
} PER_IO_OP_DATA, * LPPER_IO_OP_DATA;


void CALLBACK WorkerRoutine(DWORD dwError,
						  DWORD cbTransferred,
						  LPWSAOVERLAPPED lpOverlapped,
						  DWORD dwFlags);

void Init()
{
//	InitializeCriticalSection(&csFDRead);
	InitializeCriticalSection(&csAccept);
	InitializeCriticalSection(&csGeneralData);
	InitializeCriticalSection(&csShowMsg);
	srand(time(0));
}

void BeforeExit()
{
//	DeleteCriticalSection(&csFDRead);
	DeleteCriticalSection(&csAccept);
	DeleteCriticalSection(&csGeneralData);
	DeleteCriticalSection(&csShowMsg);
}

BOOL Add_Client(SOCKET& ClientSocket)
{
	BOOL bResult = TRUE;
	EnterCriticalSection(&csAccept);
	if(dwLeftConnect >= WSA_MAXIMUM_WAIT_EVENTS)
		bResult = FALSE;
	else
		Client[dwLeftConnect++] = ClientSocket;
	LeaveCriticalSection(&csAccept);

	return bResult;
}

void Get_Client(SOCKET& ClientSocket)
{
	EnterCriticalSection(&csAccept);
	if(dwLeftConnect > 0)
	{
		ClientSocket = Client[dwLeftConnect-1];
		Client[dwLeftConnect--] = 0;
	}
	else
		ClientSocket = INVALID_SOCKET;

	LeaveCriticalSection(&csAccept);
}

DWORD WINAPI ConnThread(LPVOID lpParam)  
{
	char Msg[1024] = "";

	ShowMsg("ConnThread start...");

//	DWORD dwCode;

	Event[EventTotal++] = ::WSACreateEvent();

	while(!bExit)
	{
		SOCKET Client = accept(Listen, NULL, NULL);
		if(Client == SOCKET_ERROR)
		{
			sprintf(Msg, "Accept error %d", ::WSAGetLastError());
			ShowMsg(Msg);
			continue;
		}
		sprintf(Msg, "Socket %d connected...", Client);
		ShowMsg(Msg);
		if(!Add_Client(Client))
		{
			closesocket(Client);
			sprintf(Msg, "The Client queue is full, can't do the request for the socket %d, close the connection.",
							Client);
			ShowMsg(Msg);
			continue;
		}
		::InterlockedIncrement((LPLONG)&TotalClients);
		
	}

	ShowMsg("ConnThread exit...");
	return 0;
}

DWORD WINAPI WorkerThread(LPVOID lpParam)
{

	char Msg[1024] = "";

	ShowMsg("WorkerThread start...");

	DWORD dwCode;
	DWORD dwIndex;
	SOCKET Client;

	DWORD dwRecv = 0;
	DWORD dwFlag = 0;

	while(!bExit)
	{
		Get_Client(Client);
		if(Client != INVALID_SOCKET)
		{
			LPPER_IO_OP_DATA lpPerIoData = (LPPER_IO_OP_DATA) GlobalAlloc(GPTR, sizeof(PER_IO_OP_DATA));
			if (NULL == lpPerIoData)
			{
				sprintf(Msg, "GlobalAlloc() failed with error %d\n", GetLastError());
				ShowMsg(Msg);
				closesocket(Client);
				continue;
			}	

			// 下一次操作置为发送操作
			lpPerIoData->dwOpType = NEXT_IO_SEND;
			lpPerIoData->DataBuf.buf = lpPerIoData->Buffer;
			lpPerIoData->DataBuf.len = BUFFER_SIZE;
			lpPerIoData->Socket = Client;

			dwFlag = 0;
			//  accept接到了数据，就放到PerIoData中,而perIoData又通过线程中的函数取出
			dwCode = WSARecv(Client, &lpPerIoData->DataBuf, 1, &dwRecv, &dwFlag,
							&(lpPerIoData->Overlapped), WorkerRoutine);
			if (dwCode == SOCKET_ERROR)
			{
				dwCode = WSAGetLastError();
				if (dwCode == WSA_IO_PENDING) continue;	// WSA_IO_PENDING == ERROR_IO_PENDING
			
				sprintf(Msg, "WSARecv() failed with error %d\n", dwCode);
				ShowMsg(Msg);
				closesocket(Client);
				::GlobalFree(lpPerIoData);
				::InterlockedDecrement((LPLONG)&TotalClients);
				continue;
			}
		}


	//
	// 需要线程处于可激活状态, 可调用 WSAWaitForMultipleEvents 或 SleepEx()
	//
//	#define USE_SLEEPEX

	#ifndef USE_SLEEPEX		
		// 1、调用 WSAWaitForMultipleEvents
//		dwIndex = ::WSAWaitForMultipleEvents(EventTotal, Event, FALSE, WSA_INFINITE, FALSE);
		dwIndex = ::WSAWaitForMultipleEvents(EventTotal, Event, FALSE, 1000, TRUE);
		if(dwIndex == WAIT_IO_COMPLETION)
		{
			ShowMsg("WSAWaitForMultipleEvents : WAIT_IO_COMPLETION");
			continue;
		}
		else
		{
			if(dwIndex == WSA_WAIT_TIMEOUT)
				continue;
			dwIndex -= WSA_WAIT_EVENT_0;
			WSAResetEvent(Event[dwIndex]);
		}
	#else
		// 2. 或者调用 SleepEx
		dwIndex = SleepEx(500, TRUE);
		if(dwIndex == WAIT_IO_COMPLETION)
		{
			ShowMsg("SleepEx : WAIT_IO_COMPLETION");
			continue;
		}
	#endif
		
	}

	ShowMsg("WorkerThread exit...");
	return 0;
}
 
void CALLBACK WorkerRoutine(DWORD dwError,
						  DWORD cbTransferred,
						  LPWSAOVERLAPPED lpOverlapped,
						  DWORD dwFlags)
{
	DWORD dwSend, dwRecv;
	DWORD dwOpFlags = 0;
	DWORD dwCode;

	LPPER_IO_OP_DATA lpPerIoData = (LPPER_IO_OP_DATA)lpOverlapped;

	char Msg[1024] = "";

	sprintf(Msg, "Total clients:%d", TotalClients);
	ShowMsg(Msg);

	if(dwError != 0 || cbTransferred == 0)
	{
		if(dwError != 0)
			sprintf(Msg, "Socket %d occured en error %d", lpPerIoData->Socket, dwError);
		else
			sprintf(Msg, "The connection for Socket %d is closed", lpPerIoData->Socket);
		ShowMsg(Msg);

		closesocket(lpPerIoData->Socket);
		::GlobalFree(lpPerIoData);
		::InterlockedDecrement((LPLONG)&TotalClients);
		return;
	}
	if(lpPerIoData->dwOpType == NEXT_IO_SEND)
	{
		sprintf(Msg, "Received data from socket %d: %s", 
					lpPerIoData->Socket, lpPerIoData->DataBuf.buf);
		ShowMsg(Msg);
		
		GetData(lpPerIoData->DataBuf.buf);
		ZeroMemory(&lpPerIoData->Overlapped, sizeof(OVERLAPPED));

		lpPerIoData->dwOpType = NEXT_IO_READ;

		dwCode = ::WSASend(lpPerIoData->Socket, &lpPerIoData->DataBuf, 1, &dwSend, dwOpFlags,
							&lpPerIoData->Overlapped, WorkerRoutine);
		if(dwCode == SOCKET_ERROR && ::WSAGetLastError() != WSA_IO_PENDING)
		{
			dwCode = ::WSAGetLastError();
			sprintf(Msg, "Do the WSARecv error for socket %d, error code %d",
						lpPerIoData->Socket, dwCode);
			ShowMsg(Msg);
			
			closesocket(lpPerIoData->Socket);
			::GlobalFree(lpPerIoData);
			::InterlockedDecrement((LPLONG)&TotalClients);
		}
	}
	else // NEXT_IO_READ
	{
		sprintf(Msg, "Sended data to socket %d: %s", 
					lpPerIoData->Socket, lpPerIoData->Buffer);
		ShowMsg(Msg);

		ZeroMemory(&lpPerIoData->Overlapped, sizeof(OVERLAPPED));
		lpPerIoData->dwOpType = NEXT_IO_SEND;

		dwRecv = 0;
		dwCode = ::WSARecv(lpPerIoData->Socket, &lpPerIoData->DataBuf, 1, &dwRecv, &dwOpFlags,
							&lpPerIoData->Overlapped, WorkerRoutine);
		if(dwCode == SOCKET_ERROR && ::WSAGetLastError() != WSA_IO_PENDING)
		{
			dwCode = ::WSAGetLastError();
			sprintf(Msg, "Do the WSARecv error for socket %d, error code %d",
						lpPerIoData->Socket, dwCode);
			ShowMsg(Msg);
			closesocket(lpPerIoData->Socket);
			::GlobalFree(lpPerIoData);
			::InterlockedDecrement((LPLONG)&TotalClients);
			
		}
	}
}

int main()
{
	int i = 0;
	DWORD dwCode;
	WSADATA wsaData;
	HANDLE hThread = NULL;

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

	Listen = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
//	Listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

	dwCode = listen(Listen, 20);
	if(dwCode == SOCKET_ERROR)
	{
		::closesocket(Listen);
		cout << "\nCan't listen:" << WSAGetLastError() << endl;
		goto EXIT;
	}

	
	hThread = ::CreateThread(NULL, NULL, ConnThread, NULL, 0, NULL);	// CREATE_SUSPENDED
	ThreadPool[nThreadCout++] = hThread;
	hThread = ::CreateThread(NULL, NULL, WorkerThread, 0, 0, NULL);
	ThreadPool[nThreadCout++] = hThread;
	

	ShowMsg("Press 'q' to exit...\n");
	while(_getch() != 'q' && _getch() != 'Q')
	{
		ShowMsg("Press 'q' to exit...\n");
	}
	bExit = TRUE;
	::WSASetEvent(Event[0]);

	::closesocket(Listen);

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