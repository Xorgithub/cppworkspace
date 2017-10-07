

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
CRITICAL_SECTION csAccept;
//CRITICAL_SECTION csFDRead;
CRITICAL_SECTION csFDWrite;

fd_set fsRead;
fd_set fsWrite;

volatile BOOL bExit  = FALSE;
int nThreadCout = 0;
volatile long dwClients = 0;




void Init()
{
	InitializeCriticalSection(&csAccept);
//	InitializeCriticalSection(&csFDRead);
	InitializeCriticalSection(&csFDWrite);
	InitializeCriticalSection(&csGeneralData);
	InitializeCriticalSection(&csShowMsg);
	srand(time(0));
}

void BeforeExit()
{
	DeleteCriticalSection(&csAccept);
//	DeleteCriticalSection(&csFDRead);
	DeleteCriticalSection(&csFDWrite);
	DeleteCriticalSection(&csGeneralData);
	DeleteCriticalSection(&csShowMsg);
}
/*

void Get_Client(SOCKET& ClientSocket)
{
	EnterCriticalSection(&csAccept);
	if(dwClients > 0)
	{
		ClientSocket = Client[dwClients-1];
		Client[dwClients--] = 0;
	}
	else
		ClientSocket = INVALID_SOCKET;

	LeaveCriticalSection(&csAccept);
}
*/

BOOL Add_Client(SOCKET& ClientSocket)
{
	BOOL bResult = TRUE;
	EnterCriticalSection(&csAccept);
	if(dwClients >= MAX_CLIENT)
	{
		ShowMsg("Can't supply the request...\n");
		closesocket(ClientSocket);
		bResult = FALSE;
	}
	else
		Client[dwClients++] = ClientSocket;
	LeaveCriticalSection(&csAccept);

	return bResult;
}

void Remove_Client(SOCKET ClientSocket)	// 客户端连接被关闭时
{
	EnterCriticalSection(&csAccept);
	int i = 0;
	for(; i < dwClients; i++)
	{
		if(ClientSocket == Client[i])
			break;
		
	}
	if(i < dwClients)
	{
		Client[i] = Client[dwClients-1];
		dwClients--;
	}

	LeaveCriticalSection(&csAccept);
}

void Set_Read_SET()
{
	EnterCriticalSection(&csAccept);

	FD_ZERO(&fsRead);
	for(int i = 0; i < dwClients; i++)
		FD_SET(Client[i], &fsRead);

	LeaveCriticalSection(&csAccept);
}

void Set_Write_SET(SOCKET ClientSocket)
{
	EnterCriticalSection(&csFDWrite);

	FD_SET(ClientSocket, &fsWrite);

	LeaveCriticalSection(&csFDWrite);
}
void Get_Write_SET(fd_set& fs)
{
	EnterCriticalSection(&csFDWrite);

	fs = fsWrite;

	LeaveCriticalSection(&csFDWrite);
}

void Remove_Client_From_Write_Set(SOCKET& ClientSocket)	// 发送完毕，调用
{
	EnterCriticalSection(&csFDWrite);

	FD_CLR(ClientSocket, &fsWrite);

	LeaveCriticalSection(&csFDWrite);
}

// 处理连接
DWORD WINAPI ConnThread(LPVOID lpParam)
{
	char Msg[BUFFER_SIZE] = "";
	ShowMsg("ConnThread start...");

	DWORD dwCode;
	fd_set FDSet;
    struct timeval timeout;

    timeout.tv_sec = 1;
    timeout.tv_usec = 100000;
    
	FD_ZERO(&fsRead);
	FD_ZERO(&fsWrite);

	while(!bExit)
	{
		FD_ZERO(&FDSet);
		FD_SET(Listen, &FDSet);
		dwCode = select(0, &FDSet, NULL, NULL, &timeout);
		if(dwCode == SOCKET_ERROR)
		{
			sprintf(Msg, "An error occured:%d\n", WSAGetLastError());
			ShowMsg(Msg);
			continue;
		}
		if(dwCode == 0) continue;	// time out

		SOCKET client = ::accept(Listen, NULL, NULL);
		sprintf(Msg, "Socket %d connected...", client);
		ShowMsg(Msg);
		if(!Add_Client(client))
			Sleep(1000);
	}

	ShowMsg("ConnThread exit...");
	return 0;
}

// 处理读数据
DWORD WINAPI ReadThread(LPVOID lpParam)  
{

	char Buf[BUFFER_SIZE] = {0};
	char Msg[BUFFER_SIZE] = "";

	ShowMsg("ReadThread start...");

	DWORD dwCode;
	fd_set FDSet;

	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 100000;

	FD_ZERO(&FDSet);

	while(!bExit)
	{
		Set_Read_SET();
	
		if(fsRead.fd_count <= 0)
		{
			Sleep(500);
			continue;
		}
		
		dwCode = select(0, &fsRead, NULL, NULL, &timeout);
		if(dwCode == SOCKET_ERROR)
		{
			sprintf(Msg, "An error coccur when waiting for reading:%d\n", WSAGetLastError());
			ShowMsg(Msg);
			continue;
		}
		else if(dwCode == 0)	// Time out
		{
			continue;
		}
		else if (dwCode > 0)	// 又数据可读
		{

			sprintf(Msg, "Total clients:%d", dwClients);
			ShowMsg(Msg);

		//	ShowMsg("Data Received...");
			for(int i = 0; i < fsRead.fd_count; i++)
			{
				dwCode = recv(fsRead.fd_array[i], Buf, BUFFER_SIZE, 0);
				if(dwCode == SOCKET_ERROR)
				{
					sprintf(Msg, "An error coccur when reading form socket %d, error code:%d\n",
								fsRead.fd_array[i], WSAGetLastError());
					ShowMsg(Msg);

					::closesocket(fsRead.fd_array[i]);
					::Remove_Client(fsRead.fd_array[i]);
					FD_CLR(fsRead.fd_array[i], &fsRead);
					continue;
				}
				else
				{
					Buf[dwCode] = '\0';
					sprintf(Msg, "Received data from socket %d:%s", fsRead.fd_array[i], Buf);
					ShowMsg(Msg);
				}
				::Set_Write_SET(fsRead.fd_array[i]);
			}
			
		}
		Sleep(500);
	}

	ShowMsg("ReadThread exit...");

	return 0;
}


// 处理写
DWORD WINAPI SendThread(LPVOID lpParam)
{

	char Buf[BUFFER_SIZE] = {0};
	char Msg[BUFFER_SIZE] = "";


	ShowMsg("SendThread start...");

	DWORD dwCode;
	fd_set fs;

	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 100000;

//	SOCKET client;
	while(!bExit)
	{
		
		Get_Write_SET(fs);
		if(fs.fd_count <= 0)
		{
			Sleep(500);
			continue;
		}
		dwCode = select(0, NULL, &fs, NULL, &timeout);
		if(dwCode == SOCKET_ERROR)
		{
			sprintf(Msg, "An error coccured when waiting for sending:%d\n", WSAGetLastError());
			ShowMsg(Msg);
			for(int i=0; i < fs.fd_count; i++)
			{
				closesocket(fs.fd_array[i]);
				::Remove_Client(fs.fd_array[i]);
				Remove_Client_From_Write_Set(fs.fd_array[i]);
			}
			continue;
		}
		else if(dwCode == 0)	// Time out
		{
			continue;
		}
		else if(dwCode > 0)	// 可以发送数据
		{
		//	ShowMsg("Sending data...");
			for(int i = 0; i < fs.fd_count; i++)
			{
				GetData(Buf);
				dwCode = send(fs.fd_array[i], Buf, 20, 0);
				if(dwCode == SOCKET_ERROR)
				{
					sprintf(Msg, "An error coccured when sending data to socket %d, error code:%d\n",
								 fs.fd_array[i], WSAGetLastError());
					ShowMsg(Msg);
					closesocket(fs.fd_array[i]);
					::Remove_Client(fs.fd_array[i]);
					continue;
				}
				sprintf(Msg, "Sended data to socket %d:%s", fs.fd_array[i], Buf);
				ShowMsg(Msg);
				Remove_Client_From_Write_Set(fs.fd_array[i]);
			}
				
		}
		Sleep(500);
	}

	ShowMsg("SendThread exit...");

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

	
	hThread = ::CreateThread(NULL, NULL, ConnThread, NULL, 0, NULL);	// CREATE_SUSPENDED
	ThreadPool[nThreadCout++] = hThread;
	hThread = ::CreateThread(NULL, NULL, ReadThread, 0, 0, NULL);
	ThreadPool[nThreadCout++] = hThread;
	hThread = ::CreateThread(NULL, NULL, SendThread, 0, 0, NULL);
	ThreadPool[nThreadCout++] = hThread;
	

	ShowMsg("Press 'q' to exit...\n");
	while(_getch() != 'q' && _getch() != 'Q')
	{
		ShowMsg("Press 'q' to exit...\n");
	}
	bExit = TRUE;

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