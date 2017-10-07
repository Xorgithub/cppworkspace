// TestClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

/*
int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}*/

#include <iostream>
#include <winsock2.h>
#include <mstcpip.h>
using namespace std;
 
#pragma comment(lib,"ws2_32.lib")

 void main2()
 {
     WSADATA wsaData;  
     WSAStartup(MAKEWORD(2,2), &wsaData);
 
	 SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

     sockaddr_in server;
     server.sin_family = AF_INET;
     server.sin_port   = htons(6000);
     server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
 
     int flag = connect(client, (sockaddr*)&server, sizeof(server));
     if(flag < 0)
	 {
         cout<<"error!"<<endl;
         return;
     }

	 struct tcp_keepalive kavars[1] = {    
		 1,    
		 10 * 1000,        /* 10 seconds */    
		 5 * 1000          /* 5 seconds */    
	 };    

	 /* Set: use keepalive on fd */    
	 int alive = 1;    
	 if (setsockopt(client, SOL_SOCKET, SO_KEEPALIVE, (const char *) &alive,    
		 sizeof alive) != 0)    
	 {    
		 printf ("Set keep alive error: %s.\n", strerror (errno));    
		 return ;    
	 }    

	 //设置定时发送keepalive检测数据包
	 DWORD ret = 0;
	 if (WSAIoctl(client, SIO_KEEPALIVE_VALS, kavars, sizeof kavars, NULL, sizeof (int), &ret, NULL,    
		 NULL) != 0)    
	 {    
		 printf("Set keep alive error: %s.\n", strerror (WSAGetLastError ()));    
		 return ;    
	 }  

     while(1)
	 {
         cout<<"sent hello!!!!"<<endl;
         char buffer[1024];
         strcpy(buffer,"hello");
         send(client, buffer, 1024, 0);
 
         memset(buffer, NULL, sizeof(buffer));
         
         cout<<"recv: "<<endl;
         int rev = recv(client, buffer, 1024, 0);
         if(rev == 0)
             cout<<"recv nothing!"<<endl;
         cout<<buffer<<endl;
         Sleep(10*1000);
     }
 
     closesocket(client);
     WSACleanup();
 }

#include <stdio.h>
#include <stdlib.h>
#define BUFF_SIZE 256
 int GetMemory(char **p, int num)
 {
	 if (p == NULL)
	 {
		 return -1;
	 }

	 *p = (char *)malloc(num);
	 if (*p == NULL)
	 {
		 return 1;
	 }
	 printf("%x\n", *p);

	 return 0;
 }
// 类似于
void func(int a)
{
	a = 5;
}

 int main( )
 {
	 char *str=NULL;
	 if (0 == GetMemory(&str, BUFF_SIZE))
	 {
		 scanf("%s", str);
		 printf("%s\n",str);
		 printf("%x\n", str);
		 free(str);
	 }

	 system("pause");
	 return 0;
 }

 