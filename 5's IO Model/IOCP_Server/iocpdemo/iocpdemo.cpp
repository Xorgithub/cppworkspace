// iocpdemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

/*
int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}*/

//实例:

//简单实现，适合IOCP入门
//参考：《WINDOWS网络与通信程序设计》



/****************************************************************** 
* 
* Copyright (c) 2008, xxxxx有限公司 
* All rights reserved. 
* 
* 文件名称：IOCPHeader.h 
* 摘   要： IOCP定义文件 
* 
* 当前版本：1.0 
* 作    者：吴会然 
* 完成日期：2008-9-16 
* 
* 取代版本： 
* 原 作者： 
* 完成日期： 
* 
******************************************************************/

#ifndef _IOCPHEADER_H_20080916_ 
#define _IOCPHEADER_H_20080916_

#include <WINSOCK2.H> 
#include <windows.h>

#pragma comment(lib,"ws2_32.lib")

#define BUFFER_SIZE 1024

/****************************************************************** 
* per_handle 数据 
*******************************************************************/ 
typedef struct _PER_HANDLE_DATA 
{ 
    SOCKET      s;      // 对应的套接字句柄 
    sockaddr_in addr;   // 对方的地址

}PER_HANDLE_DATA, *PPER_HANDLE_DATA;

/****************************************************************** 
* per_io 数据 
*******************************************************************/ 
typedef struct _PER_IO_DATA 
{ 
    OVERLAPPED ol;                 // 重叠结构 
    char        buf[BUFFER_SIZE];   // 数据缓冲区 
    int         nOperationType;     // 操作类型

#define OP_READ   1 
#define OP_WRITE 2 
#define OP_ACCEPT 3

}PER_IO_DATA, *PPER_IO_DATA;

#endif

/****************************************************************** 
* 
* Copyright (c) 2008, xxxxx有限公司 
* All rights reserved. 
* 
* 文件名称：main.cpp 
* 摘   要： iocp demo 
* 
* 当前版本：1.0 
* 作    者：吴会然 
* 完成日期：2008-9-16 
* 
* 取代版本： 
* 原 作者： 
* 完成日期： 
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
    // 创建完成端口对象 
    // 创建工作线程处理完成端口对象的事件 
    HANDLE hIocp = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, 0 ); 
    ::CreateThread( NULL, 0, ServerThread, (LPVOID)hIocp, 0, 0 );

    // 创建监听套接字，绑定本地端口，开始监听 
    SOCKET sListen = ::socket( AF_INET, SOCK_STREAM, 0 );

    SOCKADDR_IN addr; 
    addr.sin_family = AF_INET; 
    addr.sin_port = ::htons( nPort ); 
    addr.sin_addr.S_un.S_addr = INADDR_ANY; 
    ::bind( sListen, (sockaddr *)&addr, sizeof( addr ) ); 
    ::listen( sListen, 5 );

	printf("server port:%d start\n", nPort);
    // 循环处理到来的请求 
    while ( TRUE ) 
    { 
        // 等待接受未决的连接请求 
        SOCKADDR_IN saRemote; 
        int nRemoteLen = sizeof( saRemote ); 
        SOCKET sRemote = ::accept( sListen, (sockaddr *)&saRemote, &nRemoteLen );

		//lsx
		SOCKADDR_IN sockAddr; 
		int iLen=sizeof(sockAddr); 
		getpeername(sRemote, (struct sockaddr *)&sockAddr,&iLen);//得到远程IP地址和端口号  注意函数参数1：此处是接受连接   
		//socket
		char *strAddr = inet_ntoa(sockAddr.sin_addr);//IP 
		int  uIPPort = sockAddr.sin_port;//端口号
		printf("client:%s %d\n", strAddr, uIPPort);
		//lsx

        // 接受到新连接之后，为它创建一个per_handle数据，并将他们关联到完成端口对象 
        PPER_HANDLE_DATA pPerHandle = ( PPER_HANDLE_DATA )::GlobalAlloc( GPTR, sizeof( PER_HANDLE_DATA ) ); // 注意sizeof类型，而不是指针 
        if( pPerHandle == NULL ) 
        { 
            break; 
        }

        pPerHandle->s = sRemote; 
        memcpy( &pPerHandle->addr, &saRemote, nRemoteLen );

		// 将新连接套接字关联到完成端口对象
        ::CreateIoCompletionPort( ( HANDLE)pPerHandle->s, hIocp, (DWORD)pPerHandle, 0 );

        // 投递一个接受请求 
        PPER_IO_DATA pIoData = ( PPER_IO_DATA )::GlobalAlloc( GPTR, sizeof( PER_IO_DATA ) ); // 注意sizeof类型，而不是指针
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
		memset(&(pIoData->ol), 0x0, sizeof OVERLAPPED);// 注意sizeof类型，而不是指针
        ::WSARecv( pPerHandle->s, &buf, 1, &dwRecv, &dwFlags, &pIoData->ol, NULL );

    }

    ////////////////////////////////////////////////////////////////////////// 
//ERROR_PROC: 
    WSACleanup(); 
    //////////////////////////////////////////////////////////////////////////

    return 0; 
}

/****************************************************************** 
* 函数介绍：处理完成端口对象事件的线程 
* 输入参数： 
* 输出参数： 
* 返回值 ： 
*******************************************************************/ 
DWORD WINAPI ServerThread( LPVOID lpParam ) 
{
    HANDLE hIocp = ( HANDLE )lpParam; 

    DWORD dwTrans = 0; 
    PPER_HANDLE_DATA pPerHandle; 
    PPER_IO_DATA     pPerIo; 
     
    while( TRUE ) 
    { 
        // 在关联到此完成端口的所有套接字上等待I/O完成-key-socket,overlap-data buffer 
        BOOL bRet = ::GetQueuedCompletionStatus( hIocp, &dwTrans, (LPDWORD)&pPerHandle, (LPOVERLAPPED*)&pPerIo, WSA_INFINITE ); 
       
		printf("server thread:%d\n", pPerHandle->s);
		
		if( !bRet )     // 发生错误 
        { 
            ::closesocket( pPerHandle->s ); 
            ::GlobalFree( pPerHandle ); 
            ::GlobalFree( pPerIo );

            cout << "error" << endl; 
            continue; 
        }

		
        // 套接字被对方关闭 
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
        case OP_READ:       // 完成一个接收请求 
            { 
                pPerIo->buf[dwTrans] = '\0'; 
                printf( "%s\n", pPerIo->buf );

                // 继续投递接受操作 
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
