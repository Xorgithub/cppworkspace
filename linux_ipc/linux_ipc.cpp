/*
1，进程间通信 （IPC ） Inter-Process Communication
　　比较好理解概念的就是进程间通信就是在不同进程之间传播或交换信息。
  2，linux下IPC机制的分类：管道、信号、共享内存、消息队列、信号量、套接字

  3，这篇主要说说管道：本质是文件，其他理论什么的网上已经有一大堆了，我就只写一点用法吧。
  3.1 特点

  1)管道是最古老的IPC，但目前很少使用
  2）以文件做交互的媒介，管道分为有名管道和无名管道
  3）历史上的管道通常是指半双工管道


  3.2 管道：有两种形式，命令行和非命令行
  （1）命令行：

  mkfifo testfifo
  echo "testfifo" >fifo
  cat fifo

  （2）非命令行：这里又分有名管道和无名管道

  编程模型：进程A创建管道(mkfifo) -> 进程A写打开管道(open) -> 进程B读打开管道(open) -> 进程A开始往管道里写数据(write) ->

  　　　　   进程B从管道中读数据(read) -> 进程A关闭管道(close) -> 进程B关闭管道(close) -> 删除管道(unlink)
*/
//有名管道（实例）：
//进程A：

#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>

#define PIPENAME "pipetest"

int main()
{
	// 创建管道
	if(mkfifo(PIPENAME, 0666) < 0)
	{
		perror("mkfifo");
		return -1;
	}

	// 写打开管道 
	int fd = open(PIPENAME, O_WRONLY);
	if(-1 == fd)
	{
		perror("open");
		return -1;
	}

	unlink(PIPENAME);

	int i = 0;
	for(i = 0; i < 10; i++)
	{
		write(fd, &i, sizeof(i));
		printf("%d\n", i);
		sleep(1); // 这个是以秒为单位挂起
	}

	// 关闭管道
	close(fd);

	return 0;

}

//进程B：

#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>

#define PIPENAME "pipetest"

int main()
{
	// 读打开管道
	int fd = open(PIPENAME, O_RDONLY);
	if(-1 == fd)
	{
		perror("open");
		return -1;
	}

	int num = 0;
	int i = 0;
	for(i = 0; i < 10; i++)
	{
		read(fd, &num, sizeof(int));
		printf("%d\n", num);
		fflush(stdout); // 强制刷新输出缓冲区
	}

	printf("\n");
	close(fd);

	return 0;

}

/*
一，消息队列

1，概念：“消息队列”是在消息的传输过程中保存消息的容器

2，消息队列就是一个消息的链表。可以把消息看作一个记录，具有特定的格式以及特定的优先级。
　 对消息队列有写权限的进程可以向消息队列中按照一定的规则添加新消息；
   对消息队列有读权限的进程则可以从消息队列中读走消息。
   消息队列是随内核持续的。

3，编程注意事项：使用时先把数据封装成消息，把消息存入队列
	  编程步骤： 具体函数的用法可以用man手册查看（强力推荐）
	  （1）ftok()生产key
	  （2）使用msgget( ) 创建/获取消息队列，返回值是队列标识符
	  （3）使用msgsnd( ) 发送消息
	  　　 使用msgrcv( ) 接收消息
	  （4）使用msgctl( ) 删除消息队列
4，实例：

sendmsg.c   用来发送消息的
*/
// sendmsg.c

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>

struct my_msg
{
	int mtype; // 消息类型
	char buf[256];
}msg1, msg2;

int main()
{
	key_t key = ftok("./", 88);

	int msgid = msgget(key, 0666|IPC_CREAT);
	if(-1 == msgid)
	{
		perror("msgget failed!!!");
		exit(1);
	}

	msg1.mtype = 2;
	strcpy(msg1.buf, "hello, msg2");
	msgsnd(msgid, &msg1, sizeof(msg1), 0); // 阻塞
	//    msgsnd(msgid, &msg1, sizeof(msg1), IPC_NOWAIT); // 非阻塞

	msg2.mtype = 1;
	strcpy(msg2.buf, "hello, msg1");
	msgsnd(msgid, &msg2, sizeof(msg2), 0); // 阻塞

	printf("消息发送完成，按回车销毁消息队列\n");
	getchar();

	if(-1 == shmctl(msgid, IPC_RMID, NULL))
	{
		perror("shmctl failed");
		exit(2);
	}
	return 0;
}

//recvmsg.c  用来接收消息的

// recvmsg.c

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>

struct my_msg
{
	int mtype; // 消息类型
	char buf[256];
}msg;

int main()
{
	key_t key = ftok("./", 88);

	// 获取消息队列
	int msgid = msgget(key, 0);
	if(-1 == msgid)
	{
		perror("msgget failed!!!");
		exit(1);
	}

	int res = msgrcv(msgid, &msg, sizeof(msg),
		2, // 取消息类型为2的消息
		0);
	printf("类型：%d, 内容：%s\n", msg.mtype, msg.buf);

	printf("消息接收完成，按回车销毁消息队列\n");
	getchar();

	if(-1 == shmctl(msgid, IPC_RMID, NULL))
	{
		perror("shmctl failed");
		exit(2);
	}
	return 0;
}

/*
一，共享内存
内核管理一片物理内存，允许不同的进程同时映射，多个进程可以映射同一块内存，被多个进程同时映射的物理内存，即共享内存。
映射物理内存叫挂接，用完以后解除映射叫脱接。

1，共享内存的特点:
    优点：是最快的IPC。
    缺点：要编程者自己实现对共享内存互斥访问。如何实现？

2，编程模型：具体函数的用法可以用man手册查看（强力推荐）

	进程A： writeshm.c
	1) 获得key, ftok()
	2) 使用key来创建一个共享内存 shmget()
	3) 映射共享内存（得到虚拟地址）， shmat()
	4) 使用共享内存， 往共享内存中写入数据
	5) 解除映射 shmdt()
	6) 如果共享内存不再使用，可以使用shmctl()销毁共享内存

	进程B： readshm.c     
　　1) 获得key, ftok()     
  　2) 使用key来获得一个共享内存 shmget()     
	3) 映射共享内存（得到虚拟地址）， shmat()     
	4) 使用共享内存， 读取共享内存中的数据     
	5) 解除映射 shmdt()     
3，实例

*/

//进程A：
// writeshm.c

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

int main()
{
  // 生成一个key
  key_t key = ftok("./", 66);

  // 创建共享内存，返回一个id
  int shmid = shmget(key, 8, IPC_CREAT|0666|IPC_EXCL);
  if(-1 == shmid)
  {
	  perror("shmget failed");
	  exit(1);
  }

  // 映射共享内存，得到虚拟地址
  void *p = shmat(shmid, 0, 0);
  if((void*)-1 == p)
  {
	  perror("shmat failed");
	  exit(2);
  }

  // 写共享内存
  int *pp = p;
  *pp = 0x12345678;
  *(pp + 1) = 0xffffffff;

  // 解除映射
  if(-1 == shmdt(p))
  {
	  perror("shmdt failed");
	  exit(3);
  }
  printf("解除映射成功，点击回车销毁共享内存\n");
  getchar();

  // 销毁共享内存
  if(-1 == shmctl(shmid, IPC_RMID, NULL))
  {
	  perror("shmctl failed");
	  exit(4);
  }

  return 0;
}

//进程B：

// readshm.c

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

 int main()
{
  // 生成一个key
  key_t key = ftok("./", 66);

  // 获取共享内存，返回一个id
  int shmid = shmget(key, 0, 0);
  if(-1 == shmid)
  {
	  perror("shmget failed");
	  exit(1);
  }

  // 映射共享内存，得到虚拟地址
  void *p = shmat(shmid, 0, 0);
  if((void*)-1 == p)
  {
	  perror("shmat failed");
	  exit(2);
  }

  // 读共享内存
  int x = *(int *)p;
  int y = *((int *)p + 1);
  printf("从共享内存中都取了：0x%x 和 0x%x \n", x, y);

  // 解除映射
  if(-1 == shmdt(p))
  {
	  perror("shmdt failed");
	  exit(3);
  }

  return 0;
}

/*
一、信号量

1，信号量本质是一个计数器，控制访问共享资源的最大并行进程总数。（和信号有很大的区别）

2，信号量的使用主要是用来保护共享资源，使得资源在一个时刻只有一个进程（线程）所拥有。
信号量的值为正的时候，说明它空闲。所测试的线程可以锁定而使用它。若为0，说明它被占用，测试的线程要进入睡眠队列中，等待被唤醒。

3，信号量分类：Linux提供两种信号量：
（1） 内核信号量，由内核控制路径使用
（2） 用户态进程使用的信号量，这种信号量又分为POSIX信号量和SYSTEMV信号量。
POSIX信号量又分为有名信号量和无名信号量。
有名信号量，其值保存在文件中, 所以它可以用于线程也可以用于进程间的同步。无名信号量，其值保存在内存中。

干货来源：  http://blog.csdn.net/qinxiongxu/article/details/7830537

4，最简单的信号量是只能取0和1的变量，这也是信号量最常见的一种形式，叫做二进制信号量。

而可以取多个正整数的信号量被称为通用信号量。这里主要讨论二进制信号量。

5，使用方法

使用时给其一个初始值，假如该资源允许同时两个进程使用，初始值就设置为2,有一个进程使用该资源计数-1（原子操作），有一个进程放弃使用该资源计数+1（原子操作）。如果计数为0,不允许新的进程来访问资源，新的进程阻塞等待，直到计数重新大于0解除阻塞。
如果有多个资源需要控制访问，就需要多个信号量，把多个信号量存入数组中，这个数组就叫信号量集。

二，编程实现

参考： http://blog.csdn.net/ljianhui/article/details/10243617    其实就是用这篇博客的。

这里用的是二进制信号量，初始值是1，最多允许1个进程获取信号量。 

这个例子采用两个相同的程序往终端输出字符，根据命令行参数加以区分。

*/

#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/sem.h>

union semun
{
  int val;
  struct semid_ds *buf;
  unsigned short *arry;
};

static int sem_id = 0;
static int set_semvalue();
static void del_semvalue();
static int semaphore_p();
static int semaphore_v();

int main(int argc, char **argv)
{
  char message = 'x';
  int i = 0;
  // 创建信号量
  sem_id = semget((key_t)1234, 1, 0666|IPC_CREAT);

  if(argc > 1)
  {
	  // 程序第一次调用，初始化信号量
	  if(!set_semvalue())
	  {
		  fprintf(stderr, "Failed Init semaphore\n");
		  exit(EXIT_FAILURE);
	  }

	  // 设置输出到屏幕中的信息
	  message = argv[1][0];
	  sleep(2);
  }

  for(i = 0; i < 10; i++)
  {
	  if(!semaphore_p()) // 进入临界区
	  {
		  exit(EXIT_FAILURE);
	  }

	  printf("%c", message);
	  fflush(stdout); // 清理缓冲区
	  sleep(rand() % 3); // 休眠随机时间
	  printf("%c", message);
	  fflush(stdout); 

	  if(!semaphore_v()) // 离开临界区
	  {
		  exit(EXIT_FAILURE);
	  }
	  sleep(rand() % 2); // 休眠随机时间
  }

  sleep(10);
  printf("\n %d - finished\n", getpid());

  if(argc > 1)
  {
	  sleep(3);
	  del_semvalue();
  }

  exit(EXIT_SUCCESS);
}

// 初始化信号量
static int set_semvalue()
{
  union semun sem_union;
  sem_union.val = 1;
  if(-1 == semctl(sem_id, 0, SETVAL, sem_union))
  {
	  return 0;
  }
  return 1;
}

// 删除信号量 
static void del_semvalue()
{
  union semun sem_union;
  if(-1 == semctl(sem_id, 0, IPC_RMID, sem_union))
  {
	  fprintf(stderr, "Failed delete semphore\n");
  }
}

// 对信号量-1操作，即等待P(sv)
static int semaphore_p()
{
  struct sembuf sem_b;
  sem_b.sem_num = 0;
  sem_b.sem_op = -1; // P()
  sem_b.sem_flg = SEM_UNDO;

  if(-1 == semop(sem_id, &sem_b, 1))
  {
	  fprintf(stderr, "Failed semaphore_p()\n");
	  return 0;
  }

  return 1;
}

// 释放操作， +1， 发送信号V(sv)
static int semaphore_v()
{
  struct sembuf sem_b;
  sem_b.sem_num = 0;
  sem_b.sem_op = 1; // P()
  sem_b.sem_flg = SEM_UNDO;

  if(-1 == semop(sem_id, &sem_b, 1))
  {
	  fprintf(stderr, "Failed semaphore_v()\n");
	  return 0;
  }

  return 1;
}
/*
因为每个程序都在其进入临界区后和离开临界区前打印一个字符，所以每个字符都应该成对出现。
一个进程在打印时，会先执行P操作，若没有打印完，也就是没有执行V操作。另一个进程要执行打印，也要进行P操作，这时候由于信号量的值为0，获取信号量失败，进程只能挂起自己。等另一个程序释放（V操作）才能打印。
任何时刻只有一个进程得到了信号量，只有一个进程在执行打印
总结：
信号量是一个特殊的变量，程序对其访问都是原子操作，且只允许对它进行等待（即P(信号变量))和发送（即V(信号变量))信息操作。
我们通常通过信号来解决多个进程对同一资源的访问竞争的问题，使在任一时刻只能有一个执行线程访问代码的临界区域，
也可以说它是协调进程间的对同一资源的访问权，也就是用于同步进程的。
*/