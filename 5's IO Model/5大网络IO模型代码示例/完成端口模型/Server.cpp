

#include <conio.h>
#include <stdio.h>
#include <winsock2.h>
//#include <mswsock.h>


#include "Globle.h"

#pragma comment(lib, "ws2_32.lib")


SOCKET Listen;
HANDLE ThreadPool[WSA_MAXIMUM_WAIT_EVENTS] = {NULL};

HANDLE hCompletionPort = NULL;

volatile int TotalClient = 0;
volatile BOOL bExit  = FALSE;
static int nThreadCout = 0;

enum PostType{OP_RECV, OP_SEND, OP_EXIT};

typedef struct		// 第一个成员必须是OverLapped结构,根据实际需要定义自己的伪OVERLAPPED结构
{
	OVERLAPPED Overlapped;
	WSABUF DataBuf[2];
	CHAR Buffer[2][BUFFER_SIZE];
	DWORD dwOpType;

} PER_IO_OP_DATA, * LPPER_IO_OP_DATA;





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



DWORD WINAPI ConnThread(LPVOID lpParam)  
{
	char Msg[1024] = "";

	ShowMsg("ConnThread start...");

	DWORD dwCode;

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

		HANDLE hResult = CreateIoCompletionPort((HANDLE) Client, hCompletionPort, (DWORD)Client, 0);
		if(NULL  == hResult)
		{
			sprintf(Msg, "CreateIoCompletionPort() failed with error %d\n", GetLastError());
			ShowMsg(Msg);
			closesocket(Client);
			continue;
		}

	
		LPPER_IO_OP_DATA lpPerIoData = (LPPER_IO_OP_DATA) GlobalAlloc(GPTR, sizeof(PER_IO_OP_DATA));
		if (NULL == lpPerIoData)
		{
			sprintf(Msg, "GlobalAlloc() failed with error %d\n", GetLastError());
			ShowMsg(Msg);
			closesocket(Client);
			continue;
		}	
	//	ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));

		// 下一次操作置为发送操作
		lpPerIoData->dwOpType = OP_SEND;
		lpPerIoData->DataBuf[0].len = BUFFER_SIZE;
		lpPerIoData->DataBuf[0].buf = lpPerIoData->Buffer[0];

//		lpPerIoData->DataBuf[1].len = BUFFER_SIZE;
//		lpPerIoData->DataBuf[1].buf = lpPerIoData->Buffer[1];
		
		dwFlag = 0;
		
		::InterlockedIncrement((LPLONG)&TotalClient);

		//  投递一次WSARecv请求
		dwCode = WSARecv(Client, lpPerIoData->DataBuf, 1, &dwRecv, &dwFlag,
						&(lpPerIoData->Overlapped), NULL);
		if (dwCode == SOCKET_ERROR)
		{
			dwCode = WSAGetLastError();
			if (dwCode == ERROR_IO_PENDING) continue;
			
			sprintf(Msg, "WSARecv() failed with error %d\n", dwCode);
			ShowMsg(Msg);
			closesocket(Client);
			::GlobalFree(lpPerIoData);
			::InterlockedDecrement((LPLONG)&TotalClient);
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

	HANDLE hPort = (HANDLE) lpParam;
	DWORD dwTransferred;		// 传输的字节数

	SOCKET Client = INVALID_SOCKET;
	LPPER_IO_OP_DATA lpPerIoData;

	DWORD dwSendBytes, dwRecvBytes;
	DWORD dwFlag;
	DWORD dwCode;

	
	while(!bExit)
	{
		lpPerIoData = NULL;
		
		BOOL bResult = GetQueuedCompletionStatus(hPort, &dwTransferred,
											(LPDWORD)&Client,
											(LPOVERLAPPED *)&lpPerIoData,
											200);
										//	INFINITE);
		if (bResult == FALSE)
		{
			dwCode = GetLastError();
			if(dwCode == WAIT_TIMEOUT) continue;	// time out
			sprintf(Msg, "GetQueuedCompletionStatus failed with Code: %d\n", dwCode);
			ShowMsg(Msg);
			closesocket(Client);
			GlobalFree(lpPerIoData);
			::InterlockedDecrement((LPLONG)&TotalClient);

			if(dwCode == (WSAEHOSTDOWN-WSABASEERR))	// 客户端主机关闭，通常是客户端程序被关闭
			{
				sprintf(Msg, "The destination host is down\n");
				ShowMsg(Msg);
			}
			continue;
		}

		sprintf(Msg, "Total clients:%d", TotalClient);
		ShowMsg(Msg);
		if (dwTransferred == 0 && lpPerIoData->dwOpType != OP_EXIT)
		{
			sprintf(Msg, "Closing socket %d\n", Client);
			ShowMsg(Msg);

			GlobalFree(lpPerIoData);
			dwCode = closesocket(Client);
			if(dwCode == SOCKET_ERROR)
			{
				sprintf(Msg, "closesocket() failed with error %d\n", WSAGetLastError());
				ShowMsg(Msg);
			//	return 0;
			}
			else
				::InterlockedDecrement((LPLONG)&TotalClient);
			
			continue;
		}

		if(lpPerIoData->dwOpType == OP_EXIT)
		{
			closesocket(Client);
			GlobalFree(lpPerIoData);
			::InterlockedDecrement((LPLONG)&TotalClient);
			return 0;
		}
		
		dwFlag = 0;
		switch(lpPerIoData->dwOpType)
		{
		case OP_SEND:
			sprintf(Msg, "Socket %d Received:%s", Client, lpPerIoData->Buffer[0]);
			ShowMsg(Msg);
			ZeroMemory(&(lpPerIoData->Overlapped), sizeof(OVERLAPPED)); //清0为发送准备
			::GetData(lpPerIoData->Buffer[0]);
		//	lpPerIoData->DataBuf[0].buf = lpPerIoData->Buffer[0];
		//	lpPerIoData->DataBuf[0].len = BUFFER_SIZE;

			lpPerIoData->dwOpType = OP_RECV;	// 下一次操作置为接收操作

			dwCode = WSASend(Client, lpPerIoData->DataBuf, 1, &dwSendBytes, dwFlag,
							&(lpPerIoData->Overlapped), NULL);
			if(dwCode == SOCKET_ERROR)
			{
				dwCode = WSAGetLastError();
				if (dwCode == ERROR_IO_PENDING) continue;

				closesocket(Client);
				GlobalFree(lpPerIoData);
				sprintf(Msg, "WSASend() failed with error %d\n", dwCode);
				ShowMsg(Msg);
				::InterlockedDecrement((LPLONG)&TotalClient);
			}
			break;
		case OP_RECV:
			sprintf(Msg, "Sended data to Socket %d:%s", Client, lpPerIoData->Buffer[0]);
			ShowMsg(Msg);
			ZeroMemory(&(lpPerIoData->Overlapped), sizeof(OVERLAPPED));
			
		//	lpPerIoData->DataBuf[0].buf = lpPerIoData->Buffer[0];
		//	lpPerIoData->DataBuf[0].len = BUFFER_SIZE;
			lpPerIoData->dwOpType = OP_SEND;	// 下一次操作置为发送操作
			
			dwCode = WSARecv(Client, lpPerIoData->DataBuf, 1, &dwRecvBytes, &dwFlag,
							&(lpPerIoData->Overlapped), NULL);
			if(dwCode == SOCKET_ERROR)
			{
				dwCode = WSAGetLastError();
				if (dwCode == ERROR_IO_PENDING) continue;
				
				sprintf(Msg, "WSARecv() failed with error %d\n", dwCode);
				ShowMsg(Msg);
				closesocket(Client);
				GlobalFree(lpPerIoData);
				::InterlockedDecrement((LPLONG)&TotalClient);

			}
			break;
		default:
			continue;
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
	SYSTEM_INFO SystemInfo;

	Init();

	dwCode = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(dwCode != 0)
	{
		cout << "\nCan't find find a usable WinSock DLL" << endl;
		goto EXIT;
	}
	if ( LOBYTE( wsaData.wVersion ) != 2 ||  HIBYTE( wsaData.wVersion ) != 2 )
	{
		cout << "\nCan't find the socket version required." << endl;
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

	dwCode = listen(Listen, 20);
	if(dwCode == SOCKET_ERROR)
	{
		::closesocket(Listen);
		cout << "\nCan't listen:" << WSAGetLastError() << endl;
		goto EXIT;
	}
	

	hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == hCompletionPort)
	{	
		::closesocket(Listen);
		cout << "CreateIoCompletionPort failed with error" << GetLastError() << endl;
		goto EXIT;
	}


	hThread = CreateThread(NULL, 0, ConnThread, 0, 0, NULL);
	ThreadPool[nThreadCout++] = hThread;

	GetSystemInfo(&SystemInfo);

	//开CPU个数的双倍线程
	for(i = 0; (DWORD)i < SystemInfo.dwNumberOfProcessors*2; i++)
	{

		hThread = CreateThread(NULL, 0, WorkerThread, hCompletionPort, 0, NULL);
		ThreadPool[nThreadCout++] = hThread;
	}

	
	ShowMsg("Press 'q' to exit...\n");
	while(_getch() != 'q' && _getch() != 'Q')
	{
		ShowMsg("Press 'q' to exit...\n");
	}
	bExit = TRUE;
	
	::closesocket(Listen);

	::WaitForMultipleObjects(nThreadCout, ThreadPool, TRUE, INFINITE);
	::CloseHandle(hCompletionPort);

	for(i = 0; i < nThreadCout; i++)
		::CloseHandle(ThreadPool[i]);
	

EXIT:
	WSACleanup();
	BeforeExit();
	printf("\nPress any key to exit...");
	_getch();
	return 0;
}