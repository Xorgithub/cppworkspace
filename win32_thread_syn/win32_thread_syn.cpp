// win32_thread_syn.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "iostream"
#include "list"
#include "windows.h"
#include "process.h"

using namespace std;
/*
线程同步
1.临界区
2.信号量
3.事件对象
4.互斥量
*/

/************************************************************************/
/* 临界区    单进程内线程同步                                   */
/************************************************************************/
CRITICAL_SECTION g_cri;   //临界区
list<int> g_list2;
unsigned int __stdcall threadRead(LPVOID param)
{
    while(true)
    {
        EnterCriticalSection(&g_cri);
        for (list<int>::iterator it = g_list2.begin(); it != g_list2.end();it++)
        {
            cout << "元素为:"<<*it << endl;
        }
        LeaveCriticalSection(&g_cri);
        Sleep(100);
    }
    return 0;
}

unsigned int __stdcall threadWrite(LPVOID param)
{
    srand(GetTickCount());
    while (true)
    {
        EnterCriticalSection(&g_cri);
        if (g_list2.size()>0)
        {
            if (rand() % 100 > 50)
            {
                g_list2.push_back(rand());
            }
            else
            {
                g_list2.erase(g_list2.begin());
            }
        }
        else
        {
            g_list2.push_back(rand());
        }
        LeaveCriticalSection(&g_cri);
        Sleep(100);
    }
    return 0;
}

HANDLE g_mutex;  //互斥量

unsigned int times = 0;
/************************************************************************/
/*   互斥量                                                              */
/************************************************************************/
unsigned int __stdcall firstThread(LPVOID param)
{
    while (true)
    {
        WaitForSingleObject(g_mutex, INFINITE);
        if (times <=20)
        {
            cout << "线程1的第" << times++ << "次.." << endl;
        }
        else
        {
            break;
        }
        ReleaseMutex(g_mutex);
    }
    return 0;
}

unsigned int __stdcall secondThread(LPVOID param)
{
    while (true)
    {
        WaitForSingleObject(g_mutex, INFINITE);
        if (times <= 20)
        {
            Sleep(1);
            cout << "线程2的第" << times++ << "次.." << endl;
        }
        else
        {
            break;
        }
        ReleaseMutex(g_mutex);
    }
    return 0;
}


/************************************************************************/
/* 事件对象     可对进程外的线程同步   
    CreateEvent(NULL, false, false, NULL);
    setEvent(g_event);
*/
/************************************************************************/

HANDLE g_event;
unsigned int __stdcall firstThread2(LPVOID param)
{
    while (true)
    {
        WaitForSingleObject(g_event, INFINITE);
        if (times <= 20)
        {
            cout << "线程1的第" << times++ << "次.." << endl;
        }
        else
        {
            break;
        }
        SetEvent(g_event);
        
    }
    return 0;
}

unsigned int __stdcall secondThread2(LPVOID param)
{
    while (true)
    {
        WaitForSingleObject(g_event, INFINITE);
        if (times <= 20)
        {
            Sleep(1);
            cout << "线程2的第" << times++ << "次.." << endl;
        }
        else
        {
            break;
        }    
        SetEvent(g_event);
    }
    return 0;
}

/************************************************************************/
/* 信号量   主要是可以指定个数
   允许多个线程同一时刻访问同一资源，我们可以指定允许个数
*/
/************************************************************************/
HANDLE g_semaphore;

unsigned int __stdcall firstThread3(LPVOID param)
{
    while (true)
    {
        WaitForSingleObject(g_semaphore, INFINITE);
        if (times <= 20)
        {
            cout << "线程1的第" << times++ << "次.." << endl;
        }
        else
        {
            break;
        }
        ReleaseSemaphore(g_semaphore, 1, NULL);

    }
    return 0;
}

unsigned int __stdcall secondThread3(LPVOID param)
{
    while (true)
    {
        WaitForSingleObject(g_semaphore, INFINITE);
        if (times <= 20)
        {
            Sleep(1);
            cout << "线程2的第" << times++ << "次.." << endl;
        }
        else
        {
            break;
        }
        ReleaseSemaphore(g_semaphore, 1, NULL);
        
    }
    return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
    //InitializeCriticalSection(&g_cri);  //初始化临界区
    //g_mutex = CreateMutex(NULL, false, NULL);  //创建互斥量

    //g_event = CreateEvent(NULL, false, false, NULL); //可以根据第四个参数(线程名称)进行，不同进程的线程同步
    //SetEvent(g_event);

    g_semaphore = CreateSemaphore(NULL, 1, 1, NULL);   //信号量
    HANDLE g_thread1 = (HANDLE)_beginthreadex(NULL, 0, firstThread3, NULL, 0, NULL);
    HANDLE g_thread2 = (HANDLE)_beginthreadex(NULL, 0, secondThread3, NULL, 0, NULL);

    CloseHandle(g_thread1);
    CloseHandle(g_thread2);
    
    cout << "主线程..\n";
    Sleep(400000);
    
    system("pause");
    return 0;
}

