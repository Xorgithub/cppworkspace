

#include <conio.h>
#include <stdio.h>
#include <winsock2.h>
//#include <mswsock.h>


#include "Globle.h"

#pragma comment(lib, "ws2_32.lib")


//#define	MAX_CLIENT		5

#define NEXT_IO_READ		0		// 下一步是RECV 操作
#define NEXT_IO_SEND		1		// 下一步是SEND 操作

SOCKET Listen;
HANDLE ThreadPool[WSA_MAXIMUM_WAIT_EVENTS] = {NULL};

CRITICAL_SECTION csOpArray;


typedef struct 
{
	OVERLAPPED Overlapped[WSA_MAXIMUM_WAIT_EVENTS];
	WSABUF DataBuf[WSA_MAXIMUM_WAIT_EVENTS];
	char  Buffer[WSA_MAXIMUM_WAIT_EVENTS][BUFFER_SIZE];
	BOOL Buffer_Flag[WSA_MAXIMUM_WAIT_EVENTS];			// 0 --- Not Use,  1---Used;
}OVERLAPPEDDATA, *LPOVERLAPPEDDATA;



OVERLAPPEDDATA OpData;

typedef struct
{
	DWORD dwIndex;				// 标识在OpData中用到的Overlapped[i],DataBuf[i],
								// Buffer[i], Buffer_Flag[i]

	BOOL IO_Flag;				// 0 --- Read,  1---Send;
	SOCKET Socket;
	WSABUF *wsaBuf;				// 指向wIndex为索引的OpData中的 &DataBuf[dwIndwx];
	OVERLAPPED *pOverlapped;	// 指向由dwIndex为索引的OpData中的 &Overlapped[dwIndwx]
}OVERLAPPEDINFO, *LPOVERLAPPEDINFO;

OVERLAPPEDINFO OpInfo[WSA_MAXIMUM_WAIT_EVENTS];

DWORD EventTotal = 0;
WSAEVENT Event[WSA_MAXIMUM_WAIT_EVENTS]; // 每一个Event[i] 与 每个 OpInfo[i]对应

volatile BOOL bExit  = FALSE;
static int nThreadCout = 0;


void Init()
{
//	InitializeCriticalSection(&csFDRead);
	InitializeCriticalSection(&csOpArray);
	InitializeCriticalSection(&csGeneralData);
	InitializeCriticalSection(&csShowMsg);
	
	srand(time(0));

	ZeroMemory(&OpData, sizeof(OVERLAPPEDDATA));
	ZeroMemory(&OpInfo, sizeof(OVERLAPPEDINFO));
}

void BeforeExit()
{
//	DeleteCriticalSection(&csFDRead);
	DeleteCriticalSection(&csOpArray);
	DeleteCriticalSection(&csGeneralData);
	DeleteCriticalSection(&csShowMsg);
}

BOOL Do_A_Request(DWORD& dwIndex)
{
	char Msg[1024] = "";
	DWORD dwRecv = 0;
	DWORD dwFlag = 0;
	OpInfo[dwIndex].IO_Flag = NEXT_IO_SEND;

	DWORD dwCode = ::WSARecv(OpInfo[dwIndex].Socket, OpInfo[dwIndex].wsaBuf, 1, &dwRecv, &dwFlag,
							OpInfo[dwIndex].pOverlapped, NULL);
	if(dwCode == SOCKET_ERROR)
	{
		dwCode =  ::WSAGetLastError();
		if(dwCode ==  WSA_IO_PENDING) return TRUE;
		sprintf(Msg, "do the WSARecv error for socket %d, error code %d",
				OpInfo[dwIndex].Socket, dwCode);
		ShowMsg(Msg);
		return FALSE;
	}
	return TRUE;
}

BOOL AddArray(SOCKET ClientSocket, DWORD& dwIndex)
{
	::EnterCriticalSection(&csOpArray);
	if(EventTotal > WSA_MAXIMUM_WAIT_EVENTS)
	{
		::LeaveCriticalSection(&csOpArray);
		return FALSE;
	}
	int i = 0;
	for(; i < WSA_MAXIMUM_WAIT_EVENTS; i++)
		if(OpData.Buffer_Flag[i] == 0)
			break;
	
	OpData.Buffer_Flag[i]  = 1;

	WSAEVENT NewEvent = ::WSACreateEvent();
	Event[EventTotal] = NewEvent;

	OpData.DataBuf[i].buf = OpData.Buffer[i];
	OpData.DataBuf[i].len = BUFFER_SIZE;

	ZeroMemory(&OpData.Overlapped[i], sizeof(WSAOVERLAPPED));
	OpData.Overlapped[i].hEvent = NewEvent;

	OpInfo[EventTotal].dwIndex = i;
	OpInfo[EventTotal].Socket = ClientSocket;
	OpInfo[EventTotal].wsaBuf = &OpData.DataBuf[i];
	OpInfo[EventTotal].pOverlapped = &OpData.Overlapped[i];


	dwIndex = EventTotal;
	EventTotal++;
	BOOL bResult = ::Do_A_Request(dwIndex);
	::LeaveCriticalSection(&csOpArray);
	return bResult;

}

void CompressArray(DWORD dwIndex)
{

	::EnterCriticalSection(&csOpArray);
	if(dwIndex < 0 || dwIndex >= WSA_MAXIMUM_WAIT_EVENTS)
	{
		::LeaveCriticalSection(&csOpArray);
		return;
	}

	OpData.Buffer_Flag[OpInfo[dwIndex].dwIndex] = 0;
	WSAEVENT hEvent = Event[dwIndex];
	::WSACloseEvent(hEvent);
	Event[dwIndex] = Event[EventTotal-1];
	Event[EventTotal-1] = NULL;

	OpInfo[dwIndex] = OpInfo[EventTotal-1];
	ZeroMemory(&OpInfo[EventTotal-1], sizeof(OVERLAPPEDINFO));

	EventTotal--;
	::LeaveCriticalSection(&csOpArray);

}

DWORD WINAPI ConnThread(LPVOID lpParam)  
{
	char Msg[1024] = "";

	ShowMsg("ConnThread start...");

	DWORD dwIndex;
	DWORD dwRecv = 0;
	DWORD dwFlag = 0;
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

		if( !AddArray(Client, dwIndex) )
		{
			ShowMsg("Too many connections, can't do the more request!");
			closesocket(Client);
			continue;
		}
		
	}

	ShowMsg("ConnThread exit...");
	return 0;
}

DWORD WINAPI WorkerThread(LPVOID lpParam)
{

	char Buf[BUFFER_SIZE] = {0};
	char Msg[1024] = "";

	ShowMsg("WorkerThread start...");

	DWORD dwCode;

	DWORD dwIndex = 0;
	DWORD dwTransfer = 0;
	DWORD dwFlag = 0;
	while(!bExit)
	{
		if(EventTotal <= 0)
		{
			Sleep(100);
			continue;
		}
//		dwIndex = ::WSAWaitForMultipleEvents(EventTotal, Event, FALSE, WSA_INFINITE, FALSE);
		dwIndex = ::WSAWaitForMultipleEvents(EventTotal, Event, FALSE, 500, FALSE);
		if(dwIndex == WSA_WAIT_TIMEOUT)
			continue;

		sprintf(Msg, "Total clients:%d", EventTotal);
		ShowMsg(Msg);
		dwIndex -= WSA_WAIT_EVENT_0;
		WSAResetEvent(Event[dwIndex]);
		BOOL bResult = ::WSAGetOverlappedResult(OpInfo[dwIndex].Socket, OpInfo[dwIndex].pOverlapped, 
												&dwTransfer, FALSE, &dwFlag);

		if(!bResult)
		{
			dwCode = ::WSAGetLastError();
			if(dwCode == WSA_IO_PENDING)
				continue;
			sprintf(Msg, "Call WSAGetOverlappedResult for Socket %d occurs an error %d\nClose the connection", 
					OpInfo[dwIndex].Socket, dwCode);
			ShowMsg(Msg);
			
			closesocket(OpInfo[dwIndex].Socket);
			::CompressArray(dwIndex);
		}
		else
		{
			if(dwTransfer == 0)
			{
				sprintf(Msg, "The connection for Socket %d is closed", OpInfo[dwIndex].Socket);
				ShowMsg(Msg);
				closesocket(OpInfo[dwIndex].Socket);
				::CompressArray(dwIndex);
				continue;
			}
			
			if(OpInfo[dwIndex].IO_Flag == NEXT_IO_SEND)
			{
				CopyMemory(Buf, OpInfo[dwIndex].wsaBuf->buf, dwTransfer);
				Buf[dwTransfer] = '\0';
				sprintf(Msg, "Received data from socket %d: %s", OpInfo[dwIndex].Socket, Buf);
				ShowMsg(Msg);

				OpInfo[dwIndex].IO_Flag = NEXT_IO_READ;
				GetData(OpInfo[dwIndex].wsaBuf->buf);
				
				DWORD dwSend = 0;
			
				// 最好别注释下面3句代码
			//	WSAEVENT hEvent = OpInfo[dwIndex].pOverlapped->hEvent;
			//	ZeroMemory(OpInfo[dwIndex].pOverlapped, sizeof(OVERLAPPED));
			//	OpInfo[dwIndex].pOverlapped->hEvent = hEvent;

				dwCode = ::WSASend(OpInfo[dwIndex].Socket, OpInfo[dwIndex].wsaBuf, 1, &dwSend, dwFlag,
									OpInfo[dwIndex].pOverlapped, NULL);
				if(dwCode == SOCKET_ERROR && ::WSAGetLastError() != WSA_IO_PENDING)
				{
					dwCode = ::WSAGetLastError();
					sprintf(Msg, "Do the WSARecv error for socket %d, error code %d",
							OpInfo[dwIndex].Socket, dwCode);
					ShowMsg(Msg);
				//	if(dwCode == WSA_IO_PENDING || dwCode == WSAECONNRESET)
				//		continue;
					closesocket(OpInfo[dwIndex].Socket);
					::CompressArray(dwIndex);
					
				}
			}
			else		// NEXT_IO_READ
			{

				sprintf(Msg, "Sended data to socket %d: %s", OpInfo[dwIndex].Socket, OpInfo[dwIndex].wsaBuf->buf);
				ShowMsg(Msg);

				OpInfo[dwIndex].IO_Flag = NEXT_IO_SEND;
				DWORD dwRecv = 0;

				ZeroMemory(OpInfo[dwIndex].wsaBuf->buf, OpInfo[dwIndex].wsaBuf->len);
				
				// 最好别注释下面3句代码
			//	WSAEVENT hEvent = OpInfo[dwIndex].pOverlapped->hEvent;
			//	ZeroMemory(OpInfo[dwIndex].pOverlapped, sizeof(OVERLAPPED));
			//	OpInfo[dwIndex].pOverlapped->hEvent = hEvent;

				dwCode = ::WSARecv(OpInfo[dwIndex].Socket, OpInfo[dwIndex].wsaBuf, 1, &dwRecv, &dwFlag,
									OpInfo[dwIndex].pOverlapped, NULL);
				if(dwCode == SOCKET_ERROR && ::WSAGetLastError() != WSA_IO_PENDING)
				{
					dwCode = ::WSAGetLastError();
					sprintf(Msg, "Do the WSARecv error for socket %d, error code %d",
							OpInfo[dwIndex].Socket, dwCode);
					ShowMsg(Msg);
				//	if(dwCode == WSA_IO_PENDING || dwCode == WSAECONNRESET)
				//		continue;
					closesocket(OpInfo[dwIndex].Socket);
					::CompressArray(dwIndex);
					
				}
			}
		
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
//	Listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);		// the smame
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
//	hThread = ::CreateThread(NULL, NULL, SendThread, 0, 0, NULL);
//	ThreadPool[nThreadCout++] = hThread;
	

	ShowMsg("Press 'q' to exit...\n");
	while(_getch() != 'q' && _getch() != 'Q')
	{
		ShowMsg("Press 'q' to exit...\n");
	}
	bExit = TRUE;
	::WSASetEvent(Event[0]);
	for(i = 0; i < EventTotal; i++)
		closesocket(OpInfo[i].Socket);
	for(i = 1; i < EventTotal; i++)
		::WSACloseEvent(Event[i]);

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