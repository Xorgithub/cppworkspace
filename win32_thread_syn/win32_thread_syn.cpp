// win32_thread_syn.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "iostream"
#include "list"
#include "windows.h"
#include "process.h"

using namespace std;
/*
�߳�ͬ��
1.�ٽ���
2.�ź���
3.�¼�����
4.������
*/

/************************************************************************/
/* �ٽ���    ���������߳�ͬ��                                   */
/************************************************************************/
CRITICAL_SECTION g_cri;   //�ٽ���
list<int> g_list2;
unsigned int __stdcall threadRead(LPVOID param)
{
    while(true)
    {
        EnterCriticalSection(&g_cri);
        for (list<int>::iterator it = g_list2.begin(); it != g_list2.end();it++)
        {
            cout << "Ԫ��Ϊ:"<<*it << endl;
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

HANDLE g_mutex;  //������

unsigned int times = 0;
/************************************************************************/
/*   ������                                                              */
/************************************************************************/
unsigned int __stdcall firstThread(LPVOID param)
{
    while (true)
    {
        WaitForSingleObject(g_mutex, INFINITE);
        if (times <=20)
        {
            cout << "�߳�1�ĵ�" << times++ << "��.." << endl;
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
            cout << "�߳�2�ĵ�" << times++ << "��.." << endl;
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
/* �¼�����     �ɶԽ�������߳�ͬ��   
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
            cout << "�߳�1�ĵ�" << times++ << "��.." << endl;
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
            cout << "�߳�2�ĵ�" << times++ << "��.." << endl;
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
/* �ź���   ��Ҫ�ǿ���ָ������
   �������߳�ͬһʱ�̷���ͬһ��Դ�����ǿ���ָ���������
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
            cout << "�߳�1�ĵ�" << times++ << "��.." << endl;
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
            cout << "�߳�2�ĵ�" << times++ << "��.." << endl;
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
    //InitializeCriticalSection(&g_cri);  //��ʼ���ٽ���
    //g_mutex = CreateMutex(NULL, false, NULL);  //����������

    //g_event = CreateEvent(NULL, false, false, NULL); //���Ը��ݵ��ĸ�����(�߳�����)���У���ͬ���̵��߳�ͬ��
    //SetEvent(g_event);

    g_semaphore = CreateSemaphore(NULL, 1, 1, NULL);   //�ź���
    HANDLE g_thread1 = (HANDLE)_beginthreadex(NULL, 0, firstThread3, NULL, 0, NULL);
    HANDLE g_thread2 = (HANDLE)_beginthreadex(NULL, 0, secondThread3, NULL, 0, NULL);

    CloseHandle(g_thread1);
    CloseHandle(g_thread2);
    
    cout << "���߳�..\n";
    Sleep(400000);
    
    system("pause");
    return 0;
}

