

#include "stdafx.h"

/*32位Windows采用虚拟内存技术使每个进程虚拟4G内存，在逻辑上实现了对进程之间数据代码的分离与保护。那么相应的进程之间的通信也就有必要整理掌握一下。
Windows进程间通讯的方法有很多：管道、邮件槽、剪切板、共享内存、消息、套接字、RPC、DDE等。
但是他们大部分拥有一个共同的本质：利用Windows操作系统高2GB内核共享空间进行数据传递的桥梁，所以他们都是内核对象！
所以他们大部分都要遵循：A创建对象-->A写入数据-->B打开A创建的对象-->B读入数据的规则*/

//下面着重通过一些代码Demo来加深下对进程间通信的理解


//命名管道

//进程A代码

#define READ_PIPE   L"\\\\.\\pipe\\ReadPipe"
#define WRITE_PIPE  L"\\\\.\\pipe\\WritePipe"      //   管道命名

typedef struct _USER_CONTEXT_ 
{
	HANDLE hPipe;
	HANDLE hEvent;
}USER_CONTEXT,*PUSER_CONTEXT;


USER_CONTEXT  Context[2] = {0};

HANDLE hThread[2] = {0};

BOOL  WritePipe();
BOOL  ReadPipe();

BOOL  bOk = FALSE;

DWORD WINAPI WritePipeThread(LPVOID LPParam);
DWORD WINAPI ReadPipeThread(LPVOID LPParam);
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	HANDLE hPipe = NULL;
	if (WritePipe()==FALSE)
	{
		return -1;
	}
	if (ReadPipe()==FALSE)
	{

		return -1;
	}

	int iIndex = 0;
	while (TRUE)
	{    
		if (bOk==TRUE)
		{
			SetEvent(Context[0].hEvent);
			SetEvent(Context[1].hEvent);

			Sleep(1);
		}

		iIndex = WaitForMultipleObjects(2,hThread,TRUE,5000);

		if (iIndex==WAIT_TIMEOUT)
		{
			continue;
		}

		else
		{
			break;
		}

	}

	int i = 0;
	for (i=0;i<2;i++)
	{
		CloseHandle(Context[i].hEvent);
		CloseHandle(Context[i].hPipe);
	}
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	cout<<"Exit"<<endl;
	return nRetCode;
}


BOOL  WritePipe()
{
	HANDLE hWritePipe = NULL;

	hWritePipe = CreateNamedPipe( 
		WRITE_PIPE,             
		PIPE_ACCESS_DUPLEX,       
		PIPE_TYPE_MESSAGE |    
		PIPE_READMODE_MESSAGE |  
		PIPE_WAIT,               
		PIPE_UNLIMITED_INSTANCES, 
		MAX_PATH,         
		MAX_PATH,
		0,                      
		NULL);            


	if (hWritePipe==INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	Context[0].hEvent = hEvent;
	Context[0].hPipe  = hWritePipe;
	hThread[0] = CreateThread(NULL,0,WritePipeThread,NULL,0,NULL);

	return TRUE;
}



BOOL  ReadPipe()
{
	HANDLE hReadPipe = NULL;

	hReadPipe = CreateNamedPipe( 
		READ_PIPE,             
		PIPE_ACCESS_DUPLEX,       
		PIPE_TYPE_MESSAGE |    
		PIPE_READMODE_MESSAGE |  
		PIPE_WAIT,               
		PIPE_UNLIMITED_INSTANCES, 
		MAX_PATH,         
		MAX_PATH,
		0,                      
		NULL);            

	if (hReadPipe==INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	Context[1].hEvent = hEvent;
	Context[1].hPipe  = hReadPipe;
	hThread[1] = CreateThread(NULL,0,ReadPipeThread,NULL,0,NULL);

	return TRUE;

}

DWORD WINAPI ReadPipeThread(LPVOID LPParam)
{

	HANDLE hEvent     = Context[1].hEvent;
	HANDLE hReadPipe  = Context[1].hPipe;
	DWORD  dwReturn   = 0;

	char szBuffer[MAX_PATH] = {0};
	int  iIndex = 0;
	while (TRUE)
	{

		iIndex = WaitForSingleObject(hEvent,30);

		iIndex = iIndex-WAIT_OBJECT_0;

		if (iIndex==WAIT_FAILED||iIndex==0)
		{
			break;
		}

		if (ReadFile(hReadPipe,szBuffer,MAX_PATH,&dwReturn,NULL))
		{
			szBuffer[dwReturn] = '\0';

			cout<<szBuffer<<endl;
		}
		else
		{
			if (GetLastError()==ERROR_INVALID_HANDLE)
			{
				break;
			}               
		}

	}

	return 0;
}


DWORD WINAPI WritePipeThread(LPVOID LPParam)
{
	HANDLE hEvent     = Context[0].hEvent;
	HANDLE hWritePipe = Context[0].hPipe;
	DWORD  dwReturn   = 0;

	char szBuffer[MAX_PATH] = {0};
	int  iIndex = 0;
	while (TRUE)
	{
		iIndex = WaitForSingleObject(hEvent,30);

		iIndex = iIndex-WAIT_OBJECT_0;

		if (iIndex==WAIT_FAILED||iIndex==0)
		{
			break;
		}

		cin>>szBuffer;   

		if (WriteFile(hWritePipe,szBuffer,strlen(szBuffer),&dwReturn,NULL))
		{


		}

		else
		{
			if (GetLastError()==ERROR_INVALID_HANDLE)
			{
				break;
			}   
		}
	}

	return 0;
}


//进程B代码

#define WRITE_PIPE   L"\\\\.\\pipe\\ReadPipe"
#define READ_PIPE  L"\\\\.\\pipe\\WritePipe"

HANDLE hThread[2] = {0};

DWORD WINAPI  ReadPipeThread(LPARAM LPParam);
DWORD WINAPI  WritePipeThread(LPARAM LPParam);
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{


	HANDLE hReadPipe  = NULL;
	HANDLE hWritePipe = NULL;

	hThread[0] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ReadPipeThread,NULL,0,NULL);
	hThread[1] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)WritePipeThread,NULL,0,NULL);

	WaitForMultipleObjects(2,hThread,TRUE,INFINITE);

	CloseHandle(hReadPipe);
	CloseHandle(hWritePipe);

	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	cout<<"Exit"<<endl;


	return -1;
}


DWORD WINAPI WritePipeThread(LPARAM LPParam)
{
	HANDLE hWritePipe = NULL;
	char  szBuffer[MAX_PATH] = {0};
	DWORD dwReturn = 0;

	while(TRUE)
	{
		hWritePipe = CreateFile(WRITE_PIPE,GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,OPEN_EXISTING,0,NULL);


		if (hWritePipe==INVALID_HANDLE_VALUE)
		{
			continue;
		}

		break;
	}
	while (TRUE)
	{

		cin>>szBuffer;
		if (WriteFile(hWritePipe,szBuffer,MAX_PATH,&dwReturn,NULL))
		{

		}

		else
		{
			if (GetLastError()==ERROR_NO_DATA)
			{
				cout<<"Write Failed"<<endl;
				break;
			}
		}


	}
	return 0;
}


DWORD WINAPI  ReadPipeThread(LPARAM LPParam)
{

	HANDLE hReadPipe = NULL;
	char  szBuffer[MAX_PATH] = {0};
	DWORD dwReturn = 0;


	while(TRUE)
	{
		hReadPipe = CreateFile(READ_PIPE,GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,OPEN_EXISTING,0,NULL);


		if (hReadPipe==INVALID_HANDLE_VALUE)
		{
			continue;
		}

		break;
	}
	while (TRUE)
	{
		if (ReadFile(hReadPipe,szBuffer,MAX_PATH,&dwReturn,NULL))
		{
			szBuffer[dwReturn] = '\0';
			cout<<szBuffer;
		}
		else
		{
			cout<<"Read Failed"<<endl;
			break;
		}

	}
	return 0;
}


/*其中进程A创建了管道内核对象，以及用于读写管道的双线程。B进程通过对象名打开了A创建的内核对象，同时也创建了双线程进行命名管道的读与写。
对于管道需要多说的是有一种管道是匿名管道，也就是不需要创建对象管道的名字。那么其他进程又是如何知道这个管道对象，从而实现对信息的传递的呢？
原来它是通过内核对象的可继承性进行的，也就是说匿名管道只能作用于父子进程之间，在父进程创建子进程的时候通过对CreateProcess函数中传参，即可让子进程获得父进程的内核对象句柄。
具体实现细节，请参考《Windows核心编程》内核对象一章。*/

//邮件槽

//进程A代码

#define  MAIL_SLOT_NAME  L"\\\\.\\mailslot\\Name" 

HANDLE  hReadMailSlot = INVALID_HANDLE_VALUE;
DWORD WINAPI ReadMail();
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;


	HANDLE hReadThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ReadMail,NULL,0,NULL);

	Sleep(INFINITE);

	if (hReadMailSlot!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(hReadMailSlot);
	}

	Sleep(10);

	return nRetCode;
}


DWORD WINAPI ReadMail()
{

	hReadMailSlot = CreateMailslot(MAIL_SLOT_NAME,0,0,NULL);


	if (hReadMailSlot==INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	//查看油槽的信息

	DWORD cbMessage = 0;
	DWORD cMessage  = 0;
	BOOL bOk  = FALSE;
	char*  szBuffer = NULL;
	DWORD  dwReturn = 0;

	while (TRUE)
	{

		bOk =  GetMailslotInfo(hReadMailSlot,NULL,&cbMessage,&cMessage,NULL);

		if (bOk==FALSE)
		{
			break;
		}

		if (cMessage==0)
		{
			continue;
		}

		else
		{
			if (szBuffer!=NULL)
			{
				free(szBuffer);

				szBuffer = NULL;
			}

			szBuffer = (char*)malloc(sizeof(char)*cbMessage+1);


			if (ReadFile(hReadMailSlot, 
				szBuffer, 
				cbMessage, 
				&dwReturn, 
				NULL)==TRUE)
			{

				szBuffer[dwReturn] = '\0';
				if (strcmp(szBuffer,"Exit")==0)
				{
					break;
				}

				cout<<szBuffer<<endl;
			}        

		}
	}


	cout<<"ReadThread Exit"<<endl;
}

//进程B代码

#define  MAIL_SLOT_NAME  L"\\\\.\\mailslot\\Name" 
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HANDLE hWriteMailSlot = NULL;


	while(TRUE)
	{
		hWriteMailSlot = CreateFile(MAIL_SLOT_NAME,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,NULL);


		if (hWriteMailSlot==INVALID_HANDLE_VALUE)
		{
			continue;
		}
		else
		{
			break;
		}

	}

	DWORD dwReturn = 0;
	char szBuffer[1024] = {0};
	while (TRUE)
	{

		cin>>szBuffer;
		if (strcmp(szBuffer,"Exit")==0)
		{
			break;
		}

		WriteFile(hWriteMailSlot,szBuffer,strlen(szBuffer),&dwReturn,NULL);

	}

	WriteFile(hWriteMailSlot,szBuffer,strlen(szBuffer),&dwReturn,NULL);
	CloseHandle(hWriteMailSlot);

	return nRetCode;
}


/*邮件槽的实现和命名管道大同小异，都是A创建对象-->A写入数据-->B打开A创建的对象-->B读入数据。以前一直认为邮件槽是Windows与Linux共有的机制，自从某次上Liunx课和老师讨论了一会进程间通信的问题，
才愚蠢的知道Linux并没有邮件槽这个机制。*/

//共享内存

//进程A代码

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	char szBuffer[] = "Shine";

	HANDLE hMapping = CreateFileMapping(NULL,NULL,PAGE_READWRITE,0,4096,L"ShareMemory");


	LPVOID lpBase = MapViewOfFile(hMapping,FILE_MAP_WRITE|FILE_MAP_READ,0,0,0);


	strcpy((char*)lpBase,szBuffer);


	Sleep(20000);


	UnmapViewOfFile(lpBase);

	CloseHandle(hMapping);


	return nRetCode;
}

//进程B代码

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;


	HANDLE hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS,NULL,L"ShareMemory");

	if (hMapping)
	{
		wprintf(L"%s\r\n",L"Success");


		LPVOID lpBase = MapViewOfFile(hMapping,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);

		char szBuffer[20] = {0};

		strcpy(szBuffer,(char*)lpBase);


		printf("%s",szBuffer);


		UnmapViewOfFile(lpBase);

		CloseHandle(hMapping);

	}

	else
	{
		wprintf(L"%s",L"OpenMapping Error");
	}

	return nRetCode;
}


/*说道共享内存不得不说下内存映射：如何将一个文件映射到自己的缓冲区中。
打开文件-->计算文件大小-->创建内存映射对象Mapping-->mapofviewfile映射到自己的缓冲区中
通过文件映射来进行读写文件操作较为方便。*/

//文件映射代码

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HANDLE hFile = CreateFile(L"D:\\Demo.txt",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_WRITE|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	DWORD dwHigh = 0;
	DWORD dwLow = 0;
	dwLow = GetFileSize(hFile,&dwHigh);   

	dwLow = ((dwLow + 4095)/4096)*4096;

	if (hFile==INVALID_HANDLE_VALUE)
	{

		return -1;
	}

	HANDLE hMapping = CreateFileMapping(hFile,NULL,PAGE_READWRITE,dwHigh,dwLow,NULL);

	if (hMapping==NULL)
	{
		CloseHandle(hFile);
	}

	char* szBuffer = NULL;

	szBuffer = (char*)MapViewOfFile(hMapping,FILE_MAP_ALL_ACCESS,0,0,0);


	if (szBuffer!=NULL)
	{
		cout<<szBuffer<<endl;
	}
	*(szBuffer+1) = 'w';


	UnmapViewOfFile(szBuffer);

	CloseHandle(hMapping);
	CloseHandle(hFile);



	return nRetCode;
}

//消息

//进程A代码

void CServerDlg::OnBnClickedOk()
{

	CString  strBuffer;
	m_Edit.GetWindowText(strBuffer);

	if (strBuffer.GetLength()==0)
	{
		return;
	}

	COPYDATASTRUCT  Temp;

	Temp.dwData = 0;      
	Temp.cbData = strBuffer.GetLength()*sizeof(WCHAR);      //  sizeof    没有算  '\0'
	Temp.lpData = strBuffer.GetBuffer();           


	HWND hFindWindow = ::FindWindow(NULL,L"Client");

	if (hFindWindow==NULL)
	{
		return;
	}
	::SendMessage(hFindWindow,WM_COPYDATA,NULL,(LPARAM)&Temp);

}

//进程B代码

//进程B需要添加WM_COPYDATA消息

BOOL CClientDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (pCopyDataStruct->lpData==NULL||pCopyDataStruct->cbData==0)
	{
		return FALSE;
	}


	int nSize = 0;                           //字节20
	int nLen = pCopyDataStruct->cbData+sizeof(WCHAR);    //字符HelloWorld10     加了个'\0'

	WCHAR* szBuffer =  new WCHAR[nLen>>1];      //   右移一位   除以二    申请  同样大的内存

	if (szBuffer==NULL)
	{
		return FALSE;
	}
	memset(szBuffer,0,sizeof(WCHAR)*(nLen>>1));

	memcpy(szBuffer,pCopyDataStruct->lpData,pCopyDataStruct->cbData);

	m_Edit.SetWindowText(szBuffer);


	delete szBuffer;

	szBuffer = NULL;

	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}
