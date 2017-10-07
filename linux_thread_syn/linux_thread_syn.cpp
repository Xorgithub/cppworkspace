// linux_thread_syn.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

//������
#include <cstdio>  
#include <cstdlib>  
#include <unistd.h>  
#include <pthread.h>  
#include "iostream"  
using namespace std;  
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  
int tmp;  
void* thread(void *arg)  
{  
	cout << "thread id is " << pthread_self() << endl;  
	pthread_mutex_lock(&mutex);  
	tmp = 12;  
	cout << "Now a is " << tmp << endl;  
	pthread_mutex_unlock(&mutex);  
	return NULL;  
}  
int main()  
{  
	pthread_t id;  
	cout << "main thread id is " << pthread_self() << endl;  
	tmp = 3;  
	cout << "In main func tmp = " << tmp << endl;  
	if (!pthread_create(&id, NULL, thread, NULL))  
	{  
		cout << "Create thread success!" << endl;  
	}  
	else  
	{  
		cout << "Create thread failed!" << endl;  
	}  
	pthread_join(id, NULL);  
	pthread_mutex_destroy(&mutex);  
	return 0;  
}  
//���룺g++ -o thread testthread.cpp -lpthread  

//��������
#include <stdio.h>  
#include <pthread.h>  
#include "stdlib.h"  
#include "unistd.h"  
pthread_mutex_t mutex;  
pthread_cond_t cond;  
void hander(void *arg)  
{  
	free(arg);  
	(void)pthread_mutex_unlock(&mutex);  
}  
void *thread1(void *arg)  
{  
	pthread_cleanup_push(hander, &mutex);  
	while(1)  
	{  
		printf("thread1 is running\n");  
		pthread_mutex_lock(&mutex);  
		pthread_cond_wait(&cond, &mutex);  
		printf("thread1 applied the condition\n");  
		pthread_mutex_unlock(&mutex);  
		sleep(4);  
	}  
	pthread_cleanup_pop(0);  
}  
void *thread2(void *arg)  
{  
	while(1)  
	{  
		printf("thread2 is running\n");  
		pthread_mutex_lock(&mutex);  
		pthread_cond_wait(&cond, &mutex);  
		printf("thread2 applied the condition\n");  
		pthread_mutex_unlock(&mutex);  
		sleep(1);  
	}  
}  
int main()  
{  
	pthread_t thid1,thid2;  
	printf("condition variable study!\n");  
	pthread_mutex_init(&mutex, NULL);  
	pthread_cond_init(&cond, NULL);  
	pthread_create(&thid1, NULL, thread1, NULL);  
	pthread_create(&thid2, NULL, thread2, NULL);  
	sleep(1);  
	do  
	{  
		pthread_cond_signal(&cond);  
	}while(1);  
	sleep(20);  
	pthread_exit(0);  
	return 0;  
}  


#include <pthread.h>  
#include <unistd.h>  
#include "stdio.h"  
#include "stdlib.h"  
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;  
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;  
struct node  
{  
	int n_number;  
	struct node *n_next;  
}*head = NULL;  

static void cleanup_handler(void *arg)  
{  
	printf("Cleanup handler of second thread./n");  
	free(arg);  
	(void)pthread_mutex_unlock(&mtx);  
}  
static void *thread_func(void *arg)  
{  
	struct node *p = NULL;  
	pthread_cleanup_push(cleanup_handler, p);  
	while (1)  
	{  
		//���mutex��Ҫ��������֤pthread_cond_wait�Ĳ�����  
		pthread_mutex_lock(&mtx);  
		while (head == NULL)  
		{  
			//���whileҪ�ر�˵��һ�£�����pthread_cond_wait���ܺ����ƣ�Ϊ��  
			//����Ҫ��һ��while (head == NULL)�أ���Ϊpthread_cond_wait�����  
			//�̿��ܻᱻ���⻽�ѣ�������ʱ��head != NULL������������Ҫ�������  
			//���ʱ��Ӧ�����̼߳�������pthread_cond_wait  
			// pthread_cond_wait���Ƚ��֮ǰ��pthread_mutex_lock������mtx��  
			//Ȼ�������ڵȴ����������ߣ�ֱ���ٴα�����(�����������ǵȴ�����������  
			//�������ѣ����Ѻ󣬸ý��̻���������pthread_mutex_lock(&mtx);���ٶ�ȡ��Դ  
			//����������ǱȽ������  
			pthread_cond_wait(&cond, &mtx);  
			p = head;  
			head = head->n_next;  
			printf("Got %d from front of queue/n", p->n_number);  
			free(p);  
		}  
		pthread_mutex_unlock(&mtx); //�ٽ������ݲ�����ϣ��ͷŻ�����  
	}  
	pthread_cleanup_pop(0);  
	return 0;  
}  
int main(void)  
{  
	pthread_t tid;  
	int i;  
	struct node *p;  
	//���̻߳�һֱ�ȴ���Դ�����������ߺ������ߣ���������������߿����Ƕ�������ߣ���  
	//������֧����ͨ�ĵ��������ߣ����ģ����Ȼ�򵥣����Ǻ�ǿ��  
	pthread_create(&tid, NULL, thread_func, NULL);  
	sleep(1);  
	for (i = 0; i < 10; i++)  
	{  
		p = (struct node*)malloc(sizeof(struct node));  
		p->n_number = i;  
		pthread_mutex_lock(&mtx); //��Ҫ����head����ٽ���Դ���ȼ�����  
		p->n_next = head;  
		head = p;  
		pthread_cond_signal(&cond);  
		pthread_mutex_unlock(&mtx); //����  
		sleep(1);  
	}  
	printf("thread 1 wanna end the line.So cancel thread 2./n");  
	//����pthread_cancel����һ������˵�������Ǵ��ⲿ��ֹ���̣߳����̻߳��������ȡ���㣬�˳�  
	//�̣߳��������ǵĴ���������ȡ����϶�����pthread_cond_wait()�ˡ�  
	pthread_cancel(tid);  
	pthread_join(tid, NULL);  
	printf("All done -- exiting/n");  
	return 0;  
}  

//�ź���
#include <stdlib.h>  
#include <stdio.h>  
#include <unistd.h>  
#include <pthread.h>  
#include <semaphore.h>  
#include <errno.h>  
#define return_if_fail(p) if((p) == 0){printf ("[%s]:func error!/n", __func__);return;}  
typedef struct _PrivInfo  
{  
	sem_t s1;  
	sem_t s2;  
	time_t end_time;  
}PrivInfo;  

static void info_init (PrivInfo* thiz);  
static void info_destroy (PrivInfo* thiz);  
static void* pthread_func_1 (PrivInfo* thiz);  
static void* pthread_func_2 (PrivInfo* thiz);  

int main (int argc, char** argv)  
{  
	pthread_t pt_1 = 0;  
	pthread_t pt_2 = 0;  
	int ret = 0;  
	PrivInfo* thiz = NULL;  
	thiz = (PrivInfo* )malloc (sizeof (PrivInfo));  
	if (thiz == NULL)  
	{  
		printf ("[%s]: Failed to malloc priv./n");  
		return -1;  
	}  
	info_init (thiz);  
	ret = pthread_create (&pt_1, NULL, (void*)pthread_func_1, thiz);  
	if (ret != 0)  
	{  
		perror ("pthread_1_create:");  
	}  
	ret = pthread_create (&pt_2, NULL, (void*)pthread_func_2, thiz);  
	if (ret != 0)  
	{  
		perror ("pthread_2_create:");  
	}  
	pthread_join (pt_1, NULL);  
	pthread_join (pt_2, NULL);  
	info_destroy (thiz);  
	return 0;  
}  
static void info_init (PrivInfo* thiz)  
{  
	return_if_fail (thiz != NULL);  
	thiz->end_time = time(NULL) + 10;  
	sem_init (&thiz->s1, 0, 1);  
	sem_init (&thiz->s2, 0, 0);  
	return;  
}  
static void info_destroy (PrivInfo* thiz)  
{  
	return_if_fail (thiz != NULL);  
	sem_destroy (&thiz->s1);  
	sem_destroy (&thiz->s2);  
	free (thiz);  
	thiz = NULL;  
	return;  
}  
static void* pthread_func_1 (PrivInfo* thiz)  
{  
	return_if_fail(thiz != NULL);  
	while (time(NULL) < thiz->end_time)  
	{  
		sem_wait (&thiz->s2);  
		printf ("pthread1: pthread1 get the lock./n");  
		sem_post (&thiz->s1);  
		printf ("pthread1: pthread1 unlock/n");  
		sleep (1);  
	}  
	return;  
}  
static void* pthread_func_2 (PrivInfo* thiz)  
{  
	return_if_fail (thiz != NULL);  
	while (time (NULL) < thiz->end_time)  
	{  
		sem_wait (&thiz->s1);  
		printf ("pthread2: pthread2 get the unlock./n");  
		sem_post (&thiz->s2);  
		printf ("pthread2: pthread2 unlock./n");  
		sleep (1);  
	}  
	return;  
}
