

#include "stdafx.h"

/*32λWindows���������ڴ漼��ʹÿ����������4G�ڴ棬���߼���ʵ���˶Խ���֮�����ݴ���ķ����뱣������ô��Ӧ�Ľ���֮���ͨ��Ҳ���б�Ҫ��������һ�¡�
Windows���̼�ͨѶ�ķ����кࣺܶ�ܵ����ʼ��ۡ����а塢�����ڴ桢��Ϣ���׽��֡�RPC��DDE�ȡ�
�������Ǵ󲿷�ӵ��һ����ͬ�ı��ʣ�����Windows����ϵͳ��2GB�ں˹���ռ�������ݴ��ݵ��������������Ƕ����ں˶���
�������Ǵ󲿷ֶ�Ҫ��ѭ��A��������-->Aд������-->B��A�����Ķ���-->B�������ݵĹ���*/

//��������ͨ��һЩ����Demo�������¶Խ��̼�ͨ�ŵ����


//�����ܵ�

//����A����

#define READ_PIPE   L"\\\\.\\pipe\\ReadPipe"
#define WRITE_PIPE  L"\\\\.\\pipe\\WritePipe"      //   �ܵ�����

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


//����B����

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


/*���н���A�����˹ܵ��ں˶����Լ����ڶ�д�ܵ���˫�̡߳�B����ͨ������������A�������ں˶���ͬʱҲ������˫�߳̽��������ܵ��Ķ���д��
���ڹܵ���Ҫ��˵������һ�ֹܵ��������ܵ���Ҳ���ǲ���Ҫ��������ܵ������֡���ô���������������֪������ܵ����󣬴Ӷ�ʵ�ֶ���Ϣ�Ĵ��ݵ��أ�
ԭ������ͨ���ں˶���Ŀɼ̳��Խ��еģ�Ҳ����˵�����ܵ�ֻ�������ڸ��ӽ���֮�䣬�ڸ����̴����ӽ��̵�ʱ��ͨ����CreateProcess�����д��Σ��������ӽ��̻�ø����̵��ں˶�������
����ʵ��ϸ�ڣ���ο���Windows���ı�̡��ں˶���һ�¡�*/

//�ʼ���

//����A����

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

	//�鿴�Ͳ۵���Ϣ

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

//����B����

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


/*�ʼ��۵�ʵ�ֺ������ܵ���ͬС�죬����A��������-->Aд������-->B��A�����Ķ���-->B�������ݡ���ǰһֱ��Ϊ�ʼ�����Windows��Linux���еĻ��ƣ��Դ�ĳ����Liunx�κ���ʦ������һ����̼�ͨ�ŵ����⣬
���޴���֪��Linux��û���ʼ���������ơ�*/

//�����ڴ�

//����A����

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

//����B����

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


/*˵�������ڴ治�ò�˵���ڴ�ӳ�䣺��ν�һ���ļ�ӳ�䵽�Լ��Ļ������С�
���ļ�-->�����ļ���С-->�����ڴ�ӳ�����Mapping-->mapofviewfileӳ�䵽�Լ��Ļ�������
ͨ���ļ�ӳ�������ж�д�ļ�������Ϊ���㡣*/

//�ļ�ӳ�����

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

//��Ϣ

//����A����

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
	Temp.cbData = strBuffer.GetLength()*sizeof(WCHAR);      //  sizeof    û����  '\0'
	Temp.lpData = strBuffer.GetBuffer();           


	HWND hFindWindow = ::FindWindow(NULL,L"Client");

	if (hFindWindow==NULL)
	{
		return;
	}
	::SendMessage(hFindWindow,WM_COPYDATA,NULL,(LPARAM)&Temp);

}

//����B����

//����B��Ҫ���WM_COPYDATA��Ϣ

BOOL CClientDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if (pCopyDataStruct->lpData==NULL||pCopyDataStruct->cbData==0)
	{
		return FALSE;
	}


	int nSize = 0;                           //�ֽ�20
	int nLen = pCopyDataStruct->cbData+sizeof(WCHAR);    //�ַ�HelloWorld10     ���˸�'\0'

	WCHAR* szBuffer =  new WCHAR[nLen>>1];      //   ����һλ   ���Զ�    ����  ͬ������ڴ�

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
