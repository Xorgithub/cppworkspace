// iocpdemo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

/*
int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}*/

//ʵ��:

//��ʵ�֣��ʺ�IOCP����
//�ο�����WINDOWS������ͨ�ų�����ơ�



/****************************************************************** 
* 
* Copyright (c) 2008, xxxxx���޹�˾ 
* All rights reserved. 
* 
* �ļ����ƣ�IOCPHeader.h 
* ժ   Ҫ�� IOCP�����ļ� 
* 
* ��ǰ�汾��1.0 
* ��    �ߣ����Ȼ 
* ������ڣ�2008-9-16 
* 
* ȡ���汾�� 
* ԭ ���ߣ� 
* ������ڣ� 
* 
******************************************************************/

#ifndef _IOCPHEADER_H_20080916_ 
#define _IOCPHEADER_H_20080916_

#include <WINSOCK2.H> 
#include <windows.h>

#pragma comment(lib,"ws2_32.lib")

#define BUFFER_SIZE 1024

/****************************************************************** 
* per_handle ���� 
*******************************************************************/ 
typedef struct _PER_HANDLE_DATA 
{ 
    SOCKET      s;      // ��Ӧ���׽��־�� 
    sockaddr_in addr;   // �Է��ĵ�ַ

}PER_HANDLE_DATA, *PPER_HANDLE_DATA;

/****************************************************************** 
* per_io ���� 
*******************************************************************/ 
typedef struct _PER_IO_DATA 
{ 
    OVERLAPPED ol;                 // �ص��ṹ 
    char        buf[BUFFER_SIZE];   // ���ݻ����� 
    int         nOperationType;     // ��������

#define OP_READ   1 
#define OP_WRITE 2 
#define OP_ACCEPT 3

}PER_IO_DATA, *PPER_IO_DATA;

#endif

/****************************************************************** 
* 
* Copyright (c) 2008, xxxxx���޹�˾ 
* All rights reserved. 
* 
* �ļ����ƣ�main.cpp 
* ժ   Ҫ�� iocp demo 
* 
* ��ǰ�汾��1.0 
* ��    �ߣ����Ȼ 
* ������ڣ�2008-9-16 
* 
* ȡ���汾�� 
* ԭ ���ߣ� 
* ������ڣ� 
* 
******************************************************************/

#include <iostream> 
#include <string> 
//#include "IOCPHeader.h" 
using namespace std;

DWORD WINAPI ServerThread( LPVOID lpParam );

int main( int argc, char *argv[] ) 
{ 
	int nPort = 4567;

    //////////////////////////////////////////////////////////////////////////   
    WSADATA wsaData;

    if( 0 != WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) ) 
    { 
        printf( "Using %s (Status:%s)\n", wsaData.szDescription, wsaData.szSystemStatus ); 
        printf( "with API versions: %d.%d to %d.%d", 
                 LOBYTE( wsaData.wVersion), HIBYTE( wsaData.wVersion ), 
                 LOBYTE( wsaData.wHighVersion), HIBYTE( wsaData.wHighVersion) );

        return -1; 
    }
    //////////////////////////////////////////////////////////////////////////
    // ������ɶ˿ڶ��� 
    // ���������̴߳�����ɶ˿ڶ�����¼� 
    HANDLE hIocp = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, 0 ); 
    ::CreateThread( NULL, 0, ServerThread, (LPVOID)hIocp, 0, 0 );

    // ���������׽��֣��󶨱��ض˿ڣ���ʼ���� 
    SOCKET sListen = ::socket( AF_INET, SOCK_STREAM, 0 );

    SOCKADDR_IN addr; 
    addr.sin_family = AF_INET; 
    addr.sin_port = ::htons( nPort ); 
    addr.sin_addr.S_un.S_addr = INADDR_ANY; 
    ::bind( sListen, (sockaddr *)&addr, sizeof( addr ) ); 
    ::listen( sListen, 5 );

	printf("server port:%d start\n", nPort);
    // ѭ�������������� 
    while ( TRUE ) 
    { 
        // �ȴ�����δ������������ 
        SOCKADDR_IN saRemote; 
        int nRemoteLen = sizeof( saRemote ); 
        SOCKET sRemote = ::accept( sListen, (sockaddr *)&saRemote, &nRemoteLen );

		//lsx
		SOCKADDR_IN sockAddr; 
		int iLen=sizeof(sockAddr); 
		getpeername(sRemote, (struct sockaddr *)&sockAddr,&iLen);//�õ�Զ��IP��ַ�Ͷ˿ں�  ע�⺯������1���˴��ǽ�������   
		//socket
		char *strAddr = inet_ntoa(sockAddr.sin_addr);//IP 
		int  uIPPort = sockAddr.sin_port;//�˿ں�
		printf("client:%s %d\n", strAddr, uIPPort);
		//lsx

        // ���ܵ�������֮��Ϊ������һ��per_handle���ݣ��������ǹ�������ɶ˿ڶ��� 
        PPER_HANDLE_DATA pPerHandle = ( PPER_HANDLE_DATA )::GlobalAlloc( GPTR, sizeof( PER_HANDLE_DATA ) ); // ע��sizeof���ͣ�������ָ�� 
        if( pPerHandle == NULL ) 
        { 
            break; 
        }

        pPerHandle->s = sRemote; 
        memcpy( &pPerHandle->addr, &saRemote, nRemoteLen );

		// ���������׽��ֹ�������ɶ˿ڶ���
        ::CreateIoCompletionPort( ( HANDLE)pPerHandle->s, hIocp, (DWORD)pPerHandle, 0 );

        // Ͷ��һ���������� 
        PPER_IO_DATA pIoData = ( PPER_IO_DATA )::GlobalAlloc( GPTR, sizeof( PER_IO_DATA ) ); // ע��sizeof���ͣ�������ָ��
        if( pIoData == NULL ) 
        { 
            break; 
        }

        pIoData->nOperationType = OP_READ; 
        WSABUF buf; 
        buf.buf = pIoData->buf; 
        buf.len = BUFFER_SIZE; 
         
        DWORD dwRecv = 0; 
        DWORD dwFlags = 0;
		memset(&(pIoData->ol), 0x0, sizeof OVERLAPPED);// ע��sizeof���ͣ�������ָ��
        ::WSARecv( pPerHandle->s, &buf, 1, &dwRecv, &dwFlags, &pIoData->ol, NULL );

    }

    ////////////////////////////////////////////////////////////////////////// 
//ERROR_PROC: 
    WSACleanup(); 
    //////////////////////////////////////////////////////////////////////////

    return 0; 
}

/****************************************************************** 
* �������ܣ�������ɶ˿ڶ����¼����߳� 
* ��������� 
* ��������� 
* ����ֵ �� 
*******************************************************************/ 
DWORD WINAPI ServerThread( LPVOID lpParam ) 
{
    HANDLE hIocp = ( HANDLE )lpParam; 

    DWORD dwTrans = 0; 
    PPER_HANDLE_DATA pPerHandle; 
    PPER_IO_DATA     pPerIo; 
     
    while( TRUE ) 
    { 
        // �ڹ���������ɶ˿ڵ������׽����ϵȴ�I/O���-key-socket,overlap-data buffer 
        BOOL bRet = ::GetQueuedCompletionStatus( hIocp, &dwTrans, (LPDWORD)&pPerHandle, (LPOVERLAPPED*)&pPerIo, WSA_INFINITE ); 
       
		printf("server thread:%d\n", pPerHandle->s);
		
		if( !bRet )     // �������� 
        { 
            ::closesocket( pPerHandle->s ); 
            ::GlobalFree( pPerHandle ); 
            ::GlobalFree( pPerIo );

            cout << "error" << endl; 
            continue; 
        }

		
        // �׽��ֱ��Է��ر� 
        if( dwTrans == 0 && ( pPerIo->nOperationType == OP_READ || pPerIo->nOperationType/*&nb-sp;*/== OP_WRITE ) ) 
        { 
            ::closesocket( pPerHandle->s ); 
            ::GlobalFree( pPerHandle ); 
            ::GlobalFree( pPerIo );

            cout << "client closed" << endl; 
            continue; 
        }

        switch ( pPerIo->nOperationType ) 
        { 
        case OP_READ:       // ���һ���������� 
            { 
                pPerIo->buf[dwTrans] = '\0'; 
                printf( "%s\n", pPerIo->buf );

                // ����Ͷ�ݽ��ܲ��� 
                WSABUF buf; 
                buf.buf = pPerIo->buf; 
                buf.len = BUFFER_SIZE; 
                pPerIo->nOperationType = OP_READ; 
                 
                DWORD dwRecv = 0; 
                DWORD dwFlags = 0; 
                memset(&(pPerIo->ol), 0x0, sizeof OVERLAPPED);
                ::WSARecv( pPerHandle->s, &buf, 1, &dwRecv, &dwFlags, &pPerIo->ol, NULL );

            } 
            break; 
        case OP_WRITE: 
        case OP_ACCEPT: 
            break;

        }

    }

    return 0; 
}
