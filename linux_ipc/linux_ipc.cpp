/*
1�����̼�ͨ�� ��IPC �� Inter-Process Communication
�����ȽϺ�������ľ��ǽ��̼�ͨ�ž����ڲ�ͬ����֮�䴫���򽻻���Ϣ��
  2��linux��IPC���Ƶķ��ࣺ�ܵ����źš������ڴ桢��Ϣ���С��ź������׽���

  3����ƪ��Ҫ˵˵�ܵ����������ļ�����������ʲô�������Ѿ���һ����ˣ��Ҿ�ֻдһ���÷��ɡ�
  3.1 �ص�

  1)�ܵ�������ϵ�IPC����Ŀǰ����ʹ��
  2�����ļ���������ý�飬�ܵ���Ϊ�����ܵ��������ܵ�
  3����ʷ�ϵĹܵ�ͨ����ָ��˫���ܵ�


  3.2 �ܵ�����������ʽ�������кͷ�������
  ��1�������У�

  mkfifo testfifo
  echo "testfifo" >fifo
  cat fifo

  ��2���������У������ַ������ܵ��������ܵ�

  ���ģ�ͣ�����A�����ܵ�(mkfifo) -> ����Aд�򿪹ܵ�(open) -> ����B���򿪹ܵ�(open) -> ����A��ʼ���ܵ���д����(write) ->

  ��������   ����B�ӹܵ��ж�����(read) -> ����A�رչܵ�(close) -> ����B�رչܵ�(close) -> ɾ���ܵ�(unlink)
*/
//�����ܵ���ʵ������
//����A��

#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>

#define PIPENAME "pipetest"

int main()
{
	// �����ܵ�
	if(mkfifo(PIPENAME, 0666) < 0)
	{
		perror("mkfifo");
		return -1;
	}

	// д�򿪹ܵ� 
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
		sleep(1); // ���������Ϊ��λ����
	}

	// �رչܵ�
	close(fd);

	return 0;

}

//����B��

#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>

#define PIPENAME "pipetest"

int main()
{
	// ���򿪹ܵ�
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
		fflush(stdout); // ǿ��ˢ�����������
	}

	printf("\n");
	close(fd);

	return 0;

}

/*
һ����Ϣ����

1���������Ϣ���С�������Ϣ�Ĵ�������б�����Ϣ������

2����Ϣ���о���һ����Ϣ���������԰���Ϣ����һ����¼�������ض��ĸ�ʽ�Լ��ض������ȼ���
�� ����Ϣ������дȨ�޵Ľ��̿�������Ϣ�����а���һ���Ĺ����������Ϣ��
   ����Ϣ�����ж�Ȩ�޵Ľ�������Դ���Ϣ�����ж�����Ϣ��
   ��Ϣ���������ں˳����ġ�

3�����ע�����ʹ��ʱ�Ȱ����ݷ�װ����Ϣ������Ϣ�������
	  ��̲��裺 ���庯�����÷�������man�ֲ�鿴��ǿ���Ƽ���
	  ��1��ftok()����key
	  ��2��ʹ��msgget( ) ����/��ȡ��Ϣ���У�����ֵ�Ƕ��б�ʶ��
	  ��3��ʹ��msgsnd( ) ������Ϣ
	  ���� ʹ��msgrcv( ) ������Ϣ
	  ��4��ʹ��msgctl( ) ɾ����Ϣ����
4��ʵ����

sendmsg.c   ����������Ϣ��
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
	int mtype; // ��Ϣ����
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
	msgsnd(msgid, &msg1, sizeof(msg1), 0); // ����
	//    msgsnd(msgid, &msg1, sizeof(msg1), IPC_NOWAIT); // ������

	msg2.mtype = 1;
	strcpy(msg2.buf, "hello, msg1");
	msgsnd(msgid, &msg2, sizeof(msg2), 0); // ����

	printf("��Ϣ������ɣ����س�������Ϣ����\n");
	getchar();

	if(-1 == shmctl(msgid, IPC_RMID, NULL))
	{
		perror("shmctl failed");
		exit(2);
	}
	return 0;
}

//recvmsg.c  ����������Ϣ��

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
	int mtype; // ��Ϣ����
	char buf[256];
}msg;

int main()
{
	key_t key = ftok("./", 88);

	// ��ȡ��Ϣ����
	int msgid = msgget(key, 0);
	if(-1 == msgid)
	{
		perror("msgget failed!!!");
		exit(1);
	}

	int res = msgrcv(msgid, &msg, sizeof(msg),
		2, // ȡ��Ϣ����Ϊ2����Ϣ
		0);
	printf("���ͣ�%d, ���ݣ�%s\n", msg.mtype, msg.buf);

	printf("��Ϣ������ɣ����س�������Ϣ����\n");
	getchar();

	if(-1 == shmctl(msgid, IPC_RMID, NULL))
	{
		perror("shmctl failed");
		exit(2);
	}
	return 0;
}

/*
һ�������ڴ�
�ں˹���һƬ�����ڴ棬����ͬ�Ľ���ͬʱӳ�䣬������̿���ӳ��ͬһ���ڴ棬���������ͬʱӳ��������ڴ棬�������ڴ档
ӳ�������ڴ�йҽӣ������Ժ���ӳ����ѽӡ�

1�������ڴ���ص�:
    �ŵ㣺������IPC��
    ȱ�㣺Ҫ������Լ�ʵ�ֶԹ����ڴ滥����ʡ����ʵ�֣�

2�����ģ�ͣ����庯�����÷�������man�ֲ�鿴��ǿ���Ƽ���

	����A�� writeshm.c
	1) ���key, ftok()
	2) ʹ��key������һ�������ڴ� shmget()
	3) ӳ�乲���ڴ棨�õ������ַ���� shmat()
	4) ʹ�ù����ڴ棬 �������ڴ���д������
	5) ���ӳ�� shmdt()
	6) ��������ڴ治��ʹ�ã�����ʹ��shmctl()���ٹ����ڴ�

	����B�� readshm.c     
����1) ���key, ftok()     
  ��2) ʹ��key�����һ�������ڴ� shmget()     
	3) ӳ�乲���ڴ棨�õ������ַ���� shmat()     
	4) ʹ�ù����ڴ棬 ��ȡ�����ڴ��е�����     
	5) ���ӳ�� shmdt()     
3��ʵ��

*/

//����A��
// writeshm.c

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

int main()
{
  // ����һ��key
  key_t key = ftok("./", 66);

  // ���������ڴ棬����һ��id
  int shmid = shmget(key, 8, IPC_CREAT|0666|IPC_EXCL);
  if(-1 == shmid)
  {
	  perror("shmget failed");
	  exit(1);
  }

  // ӳ�乲���ڴ棬�õ������ַ
  void *p = shmat(shmid, 0, 0);
  if((void*)-1 == p)
  {
	  perror("shmat failed");
	  exit(2);
  }

  // д�����ڴ�
  int *pp = p;
  *pp = 0x12345678;
  *(pp + 1) = 0xffffffff;

  // ���ӳ��
  if(-1 == shmdt(p))
  {
	  perror("shmdt failed");
	  exit(3);
  }
  printf("���ӳ��ɹ�������س����ٹ����ڴ�\n");
  getchar();

  // ���ٹ����ڴ�
  if(-1 == shmctl(shmid, IPC_RMID, NULL))
  {
	  perror("shmctl failed");
	  exit(4);
  }

  return 0;
}

//����B��

// readshm.c

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

 int main()
{
  // ����һ��key
  key_t key = ftok("./", 66);

  // ��ȡ�����ڴ棬����һ��id
  int shmid = shmget(key, 0, 0);
  if(-1 == shmid)
  {
	  perror("shmget failed");
	  exit(1);
  }

  // ӳ�乲���ڴ棬�õ������ַ
  void *p = shmat(shmid, 0, 0);
  if((void*)-1 == p)
  {
	  perror("shmat failed");
	  exit(2);
  }

  // �������ڴ�
  int x = *(int *)p;
  int y = *((int *)p + 1);
  printf("�ӹ����ڴ��ж�ȡ�ˣ�0x%x �� 0x%x \n", x, y);

  // ���ӳ��
  if(-1 == shmdt(p))
  {
	  perror("shmdt failed");
	  exit(3);
  }

  return 0;
}

/*
һ���ź���

1���ź���������һ�������������Ʒ��ʹ�����Դ������н��������������ź��кܴ������

2���ź�����ʹ����Ҫ����������������Դ��ʹ����Դ��һ��ʱ��ֻ��һ�����̣��̣߳���ӵ�С�
�ź�����ֵΪ����ʱ��˵�������С������Ե��߳̿���������ʹ��������Ϊ0��˵������ռ�ã����Ե��߳�Ҫ����˯�߶����У��ȴ������ѡ�

3���ź������ࣺLinux�ṩ�����ź�����
��1�� �ں��ź��������ں˿���·��ʹ��
��2�� �û�̬����ʹ�õ��ź����������ź����ַ�ΪPOSIX�ź�����SYSTEMV�ź�����
POSIX�ź����ַ�Ϊ�����ź����������ź�����
�����ź�������ֵ�������ļ���, ���������������߳�Ҳ�������ڽ��̼��ͬ���������ź�������ֵ�������ڴ��С�

�ɻ���Դ��  http://blog.csdn.net/qinxiongxu/article/details/7830537

4����򵥵��ź�����ֻ��ȡ0��1�ı�������Ҳ���ź��������һ����ʽ�������������ź�����

������ȡ������������ź�������Ϊͨ���ź�����������Ҫ���۶������ź�����

5��ʹ�÷���

ʹ��ʱ����һ����ʼֵ���������Դ����ͬʱ��������ʹ�ã���ʼֵ������Ϊ2,��һ������ʹ�ø���Դ����-1��ԭ�Ӳ���������һ�����̷���ʹ�ø���Դ����+1��ԭ�Ӳ��������������Ϊ0,�������µĽ�����������Դ���µĽ��������ȴ���ֱ���������´���0���������
����ж����Դ��Ҫ���Ʒ��ʣ�����Ҫ����ź������Ѷ���ź������������У��������ͽ��ź�������

�������ʵ��

�ο��� http://blog.csdn.net/ljianhui/article/details/10243617    ��ʵ��������ƪ���͵ġ�

�����õ��Ƕ������ź�������ʼֵ��1���������1�����̻�ȡ�ź����� 

������Ӳ���������ͬ�ĳ������ն�����ַ������������в����������֡�

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
  // �����ź���
  sem_id = semget((key_t)1234, 1, 0666|IPC_CREAT);

  if(argc > 1)
  {
	  // �����һ�ε��ã���ʼ���ź���
	  if(!set_semvalue())
	  {
		  fprintf(stderr, "Failed Init semaphore\n");
		  exit(EXIT_FAILURE);
	  }

	  // �����������Ļ�е���Ϣ
	  message = argv[1][0];
	  sleep(2);
  }

  for(i = 0; i < 10; i++)
  {
	  if(!semaphore_p()) // �����ٽ���
	  {
		  exit(EXIT_FAILURE);
	  }

	  printf("%c", message);
	  fflush(stdout); // ��������
	  sleep(rand() % 3); // �������ʱ��
	  printf("%c", message);
	  fflush(stdout); 

	  if(!semaphore_v()) // �뿪�ٽ���
	  {
		  exit(EXIT_FAILURE);
	  }
	  sleep(rand() % 2); // �������ʱ��
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

// ��ʼ���ź���
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

// ɾ���ź��� 
static void del_semvalue()
{
  union semun sem_union;
  if(-1 == semctl(sem_id, 0, IPC_RMID, sem_union))
  {
	  fprintf(stderr, "Failed delete semphore\n");
  }
}

// ���ź���-1���������ȴ�P(sv)
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

// �ͷŲ����� +1�� �����ź�V(sv)
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
��Ϊÿ��������������ٽ�������뿪�ٽ���ǰ��ӡһ���ַ�������ÿ���ַ���Ӧ�óɶԳ��֡�
һ�������ڴ�ӡʱ������ִ��P��������û�д�ӡ�꣬Ҳ����û��ִ��V��������һ������Ҫִ�д�ӡ��ҲҪ����P��������ʱ�������ź�����ֵΪ0����ȡ�ź���ʧ�ܣ�����ֻ�ܹ����Լ�������һ�������ͷţ�V���������ܴ�ӡ��
�κ�ʱ��ֻ��һ�����̵õ����ź�����ֻ��һ��������ִ�д�ӡ
�ܽ᣺
�ź�����һ������ı��������������ʶ���ԭ�Ӳ�������ֻ����������еȴ�����P(�źű���))�ͷ��ͣ���V(�źű���))��Ϣ������
����ͨ��ͨ���ź������������̶�ͬһ��Դ�ķ��ʾ��������⣬ʹ����һʱ��ֻ����һ��ִ���̷߳��ʴ�����ٽ�����
Ҳ����˵����Э�����̼�Ķ�ͬһ��Դ�ķ���Ȩ��Ҳ��������ͬ�����̵ġ�
*/