�嵥 1. �򿪼����׽��֣�ע��һ���ص�������ÿ����Ҫ���� accept() �����Դ�������ʱ�����������ɴ˴������������

				
int main(int argc, char **argv)
{
...
    ev_init();

    /* Setup listening socket */

    event_set(&ev_accept, listen_fd, EV_READ|EV_PERSIST, on_accept, NULL);
    event_add(&ev_accept, NULL);

    /* Start the event loop. */
    event_dispatch();
}

 

event_set() ���������µ��¼��ṹ��event_add() ���¼����л���������¼���Ȼ��event_dispatch() �����¼�����ϵͳ����ʼ�����������ܣ�����

�嵥 2 ����һ����������ʾ����������һ���ǳ��򵥵Ļ��Է�������


�嵥 2. �����򵥵Ļ��Է�����

				
#include <event.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define SERVER_PORT 8080
int debug = 0;

struct client {
  int fd;
  struct bufferevent *buf_ev;
};

int setnonblock(int fd)
{
  int flags;

  flags = fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(fd, F_SETFL, flags);
}

void buf_read_callback(struct bufferevent *incoming,
                       void *arg)
{
  struct evbuffer *evreturn;
  char *req;

  req = evbuffer_readline(incoming->input);
  if (req == NULL)
    return;

  evreturn = evbuffer_new();
  evbuffer_add_printf(evreturn,"You said %s\n",req);
  bufferevent_write_buffer(incoming,evreturn);
  evbuffer_free(evreturn);
  free(req);
}

void buf_write_callback(struct bufferevent *bev,
                        void *arg)
{
}

void buf_error_callback(struct bufferevent *bev,
                        short what,
                        void *arg)
{
  struct client *client = (struct client *)arg;
  bufferevent_free(client->buf_ev);
  close(client->fd);
  free(client);
}

void accept_callback(int fd,
                     short ev,
                     void *arg)
{
  int client_fd;
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  struct client *client;

  client_fd = accept(fd,
                     (struct sockaddr *)&client_addr,
                     &client_len);
  if (client_fd < 0)
    {
      warn("Client: accept() failed");
      return;
    }

  setnonblock(client_fd);

  client = calloc(1, sizeof(*client));
  if (client == NULL)
    err(1, "malloc failed");
  client->fd = client_fd;

  client->buf_ev = bufferevent_new(client_fd,
                                   buf_read_callback,
                                   buf_write_callback,
                                   buf_error_callback,
                                   client);

  bufferevent_enable(client->buf_ev, EV_READ);
}

int main(int argc,
         char **argv)
{
  int socketlisten;
  struct sockaddr_in addresslisten;
  struct event accept_event;
  int reuse = 1;

  event_init();

  socketlisten = socket(AF_INET, SOCK_STREAM, 0);

  if (socketlisten < 0)
    {
      fprintf(stderr,"Failed to create listen socket");
      return 1;
    }

  memset(&addresslisten, 0, sizeof(addresslisten));

  addresslisten.sin_family = AF_INET;
  addresslisten.sin_addr.s_addr = INADDR_ANY;
  addresslisten.sin_port = htons(SERVER_PORT);

  if (bind(socketlisten,
           (struct sockaddr *)&addresslisten,
           sizeof(addresslisten)) < 0)
    {
      fprintf(stderr,"Failed to bind");
      return 1;
    }

  if (listen(socketlisten, 5) < 0)
    {
      fprintf(stderr,"Failed to listen to socket");
      return 1;
    }

  setsockopt(socketlisten,
             SOL_SOCKET,
             SO_REUSEADDR,
             &reuse,
             sizeof(reuse));

  setnonblock(socketlisten);

  event_set(&accept_event,
            socketlisten,
            EV_READ|EV_PERSIST,
            accept_callback,
            NULL);

  event_add(&accept_event,
            NULL);

  event_dispatch();

  close(socketlisten);

  return 0;
}

 

�������۸����������������

    main()�����������������������ӵ��׽��֣�Ȼ�󴴽� accept() �Ļص������Ա�ͨ���¼�����������ÿ�����ӡ�
    accept_callback()������������ʱ���¼�ϵͳ���ô˺������˺������ܵ��ͻ��˵����ӣ���ӿ� �����׽�����Ϣ��һ�� bufferevent �ṹ�����¼��ṹ��Ϊ�ͻ����׽����ϵĶ�/д/�����¼���ӻص���������Ϊ�������ݿͻ��˽ṹ����Ƕ��� eventbuffer �Ϳͻ����׽��֣���ÿ����Ӧ�Ŀͻ����׽��ְ�������д��������ʱ�����ö�Ӧ�Ļص�������
    buf_read_callback()�����ͻ����׽�����Ҫ��������ʱ����������Ϊ���Է��񣬴˺����� "you said..." д�ؿͻ��ˡ��׽�����Ȼ�򿪣����Խ���������
    buf_write_callback()������Ҫд������ʱ��������������򵥵ķ����У�����Ҫ�˺��������Զ����ǿյġ�
    buf_error_callback()�������ִ���ʱ��������������ͻ����ж����ӡ��ڳ��ִ�������г����У��رտͻ����׽��֣����¼��б���ɾ���ͻ����׽��ֵ��¼���Ŀ���ͷſͻ��˽ṹ���ڴ档
    setnonblock()�����������׽����Կ��� I/O�� 

���ͻ�������ʱ�����¼�������������¼��Դ���ͻ������ӣ����ͻ����ж�����ʱɾ���¼�����Ļ��libevent ���������׽��֣�ʶ����Ҫ����Ŀͻ��ˣ��ֱ���ö�Ӧ�ĺ�����

Ϊ�˹������Ӧ�ó�����Ҫ���� C Դ���벢��� libevent �⣺$ gcc -o basic basic.c -levent��

�ӿͻ��˵ĽǶ���������������������ѷ��͸������κ��ı����ͻ������� �嵥 3����


�嵥 3. �������ѷ��͸������ı����ͻ���

				
$ telnet localhost 8080
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Hello!
You said Hello!

����������Ӧ�ó���ǳ��ʺ���Ҫ���������ӵĴ��ģ�ֲ�ʽ���𣬱��� IBM Cloud ϵͳ��

����ͨ���򵥵Ľ�������۲촦������������ӵ���������ܸĽ�������ʹ��Ƕ��� HTTP ʵ�ְ����˽�������ԡ�

ʹ�����õ� HTTP ������

���ϣ����������Ӧ�ó��򣬿���ʹ��һ��Ļ�������� libevent �ӿڣ����ǣ�Խ��Խ�����ĳ����ǿ������� HTTP Э���Ӧ�ó����Լ�װ�ػ�̬������װ����Ϣ����ҳ�����ʹ���κ� AJAX �⣬�ͻ��˾���Ҫ HTTP����ʹ�����ص���Ϣ�� XML �� JSON��

libevent �е� HTTP ʵ�ֲ����� Apache HTTP �����������Ʒ���������������ƺ� web ����������Ĵ��ģ��̬���ݵ�ʵ�ý�����������磬������ IBM Cloud ��������������в������ libevent �Ľӿڡ���Ϊ����ʹ�� HTTP ����ͨ�ţ�����������������������ɡ�

Ҫ��ʹ�� libevent ������Ҫʹ������Ҫ�����¼�ģ����ͬ�Ļ����ṹ�����ǻ����봦������ӿڣ�HTTP ��װ��������������ʹ�������̱���ĸ��������ã���ʼ�������� HTTP ������������ HTTP �ص������ͽ����¼�ѭ�������ټ��Ϸ��ͻ����ݵĻص��������嵥 4 ����һ���ǳ��򵥵�ʾ����


�嵥 4. ʹ�� libevent ����ļ�ʾ��

				
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <event.h>
#include <evhttp.h>

void generic_request_handler(struct evhttp_request *req, void *arg)
{
  struct evbuffer *returnbuffer = evbuffer_new();

  evbuffer_add_printf(returnbuffer, "Thanks for the request!");
  evhttp_send_reply(req, HTTP_OK, "Client", returnbuffer);
  evbuffer_free(returnbuffer);
  return;
}

int main(int argc, char **argv)
{
  short          http_port = 8081;
  char          *http_addr = "192.168.0.22";
  struct evhttp *http_server = NULL;

  event_init();
  http_server = evhttp_start(http_addr, http_port);
  evhttp_set_gencb(http_server, generic_request_handler, NULL);

  fprintf(stderr, "Server started on port %d\n", http_port);
  event_dispatch();

  return(0);
}

 

Ӧ�ÿ���ͨ��ǰ���ʾ����������Ļ����ṹ������Ҫ���͡���ҪԪ���� evhttp_set_gencb() �����������õ��յ� HTTP ����ʱҪʹ�õĻص��������� generic_request_handler() �ص�������������һ����ʾ�ɹ��ļ���Ϣ�����Ӧ����������

HTTP ��װ���ṩ����������ܡ����磬��һ�����������������ӵ��͵���������ȡ����ѯ������������ CGI ����һ�����������������ڲ�ͬ������·����Ҫ�����Ĵ�������ͨ�����ò�ͬ�Ļص������ʹ�����������ʹ��·�� '/db/' �ṩ�����ݿ�Ľӿڣ���ʹ�� '/memc' �ṩ�� memcached �Ľӿڡ�

libevent ���߰�����һ��������֧��ͨ�ü�ʱ����������ָ����ʱ���֮�󴥷��¼�������ͨ�����ʹ�ü�ʱ���� HTTP ʵ���ṩ�����ķ��񣬴Ӷ��Զ����ṩ�ļ����ݣ����޸��ļ�����ʱ���·��ص����ݡ����磬��ǰҪ��������Ƶ���Ļ�ڼ��ṩ��ʱ���·���ǰ�� web Ӧ�ó������Ҫ��������װ�����Ÿ壬�����ڿ������ɵ��ṩ���ݡ�����Ӧ�ó��򣨺� web ���񣩶����ڴ��У������Ӧ�ǳ��졣

����� �嵥 5 �е�ʾ������Ҫ��;��


�嵥 5. ʹ�ü�ʱ��������Ƶ���Ļ�ڼ��ṩ��ʱ���·���

				
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <event.h>
#include <evhttp.h>

#define RELOAD_TIMEOUT 5
#define DEFAULT_FILE "sample.html"

char *filedata;
time_t lasttime = 0;
char filename[80];
int counter = 0;

void read_file()
{
  int size = 0;
  char *data;
  struct stat buf;

  stat(filename,&buf);

  if (buf.st_mtime > lasttime)
    {
      if (counter++)
        fprintf(stderr,"Reloading file: %s",filename);
      else
        fprintf(stderr,"Loading file: %s",filename);

      FILE *f = fopen(filename, "rb");
      if (f == NULL)
        {
          fprintf(stderr,"Couldn't open file\n");
          exit(1);
        }

      fseek(f, 0, SEEK_END);
      size = ftell(f);
      fseek(f, 0, SEEK_SET);
      data = (char *)malloc(size+1);
      fread(data, sizeof(char), size, f);
      filedata = (char *)malloc(size+1);
      strcpy(filedata,data);
      fclose(f);


      fprintf(stderr," (%d bytes)\n",size);
      lasttime = buf.st_mtime;
    }
}

void load_file()
{
  struct event *loadfile_event;
  struct timeval tv;

  read_file();

  tv.tv_sec = RELOAD_TIMEOUT;
  tv.tv_usec = 0;

  loadfile_event = malloc(sizeof(struct event));

  evtimer_set(loadfile_event,
              load_file,
              loadfile_event);

  evtimer_add(loadfile_event,
              &tv);
}

void generic_request_handler(struct evhttp_request *req, void *arg)
{
  struct evbuffer *evb = evbuffer_new();

  evbuffer_add_printf(evb, "%s",filedata);
  evhttp_send_reply(req, HTTP_OK, "Client", evb);
  evbuffer_free(evb);
}

int main(int argc, char *argv[])
{
  short          http_port = 8081;
  char          *http_addr = "192.168.0.22";
  struct evhttp *http_server = NULL;

  if (argc > 1)
    {
      strcpy(filename,argv[1]);
      printf("Using %s\n",filename);
    }
  else
    {
      strcpy(filename,DEFAULT_FILE);
    }

  event_init();

  load_file();

  http_server = evhttp_start(http_addr, http_port);
  evhttp_set_gencb(http_server, generic_request_handler, NULL);

  fprintf(stderr, "Server started on port %d\n", http_port);
  event_dispatch();
}
