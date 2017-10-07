// linux_thread_syn.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

//互斥量
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
//编译：g++ -o thread testthread.cpp -lpthread  

//条件变量
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
		//这个mutex主要是用来保证pthread_cond_wait的并发性  
		pthread_mutex_lock(&mtx);  
		while (head == NULL)  
		{  
			//这个while要特别说明一下，单个pthread_cond_wait功能很完善，为何  
			//这里要有一个while (head == NULL)呢？因为pthread_cond_wait里的线  
			//程可能会被意外唤醒，如果这个时候head != NULL，则不是我们想要的情况。  
			//这个时候，应该让线程继续进入pthread_cond_wait  
			// pthread_cond_wait会先解除之前的pthread_mutex_lock锁定的mtx，  
			//然后阻塞在等待对列里休眠，直到再次被唤醒(大多数情况下是等待的条件成立  
			//而被唤醒，唤醒后，该进程会先锁定先pthread_mutex_lock(&mtx);，再读取资源  
			//用这个流程是比较清楚的  
			pthread_cond_wait(&cond, &mtx);  
			p = head;  
			head = head->n_next;  
			printf("Got %d from front of queue/n", p->n_number);  
			free(p);  
		}  
		pthread_mutex_unlock(&mtx); //临界区数据操作完毕，释放互斥锁  
	}  
	pthread_cleanup_pop(0);  
	return 0;  
}  
int main(void)  
{  
	pthread_t tid;  
	int i;  
	struct node *p;  
	//子线程会一直等待资源，类似生产者和消费者，但是这里的消费者可以是多个消费者，而  
	//不仅仅支持普通的单个消费者，这个模型虽然简单，但是很强大  
	pthread_create(&tid, NULL, thread_func, NULL);  
	sleep(1);  
	for (i = 0; i < 10; i++)  
	{  
		p = (struct node*)malloc(sizeof(struct node));  
		p->n_number = i;  
		pthread_mutex_lock(&mtx); //需要操作head这个临界资源，先加锁，  
		p->n_next = head;  
		head = p;  
		pthread_cond_signal(&cond);  
		pthread_mutex_unlock(&mtx); //解锁  
		sleep(1);  
	}  
	printf("thread 1 wanna end the line.So cancel thread 2./n");  
	//关于pthread_cancel，有一点额外的说明，它是从外部终止子线程，子线程会在最近的取消点，退出  
	//线程，而在我们的代码里，最近的取消点肯定就是pthread_cond_wait()了。  
	pthread_cancel(tid);  
	pthread_join(tid, NULL);  
	printf("All done -- exiting/n");  
	return 0;  
}  

//信号量
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
