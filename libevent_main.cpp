清单 1. 打开监听套接字，注册一个回调函数（每当需要调用 accept() 函数以打开新连接时调用它），由此创建网络服务器

				
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

 

event_set() 函数创建新的事件结构，event_add() 在事件队列机制中添加事件。然后，event_dispatch() 启动事件队列系统，开始监听（并接受）请求。

清单 2 给出一个更完整的示例，它构建一个非常简单的回显服务器：


清单 2. 构建简单的回显服务器

				
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

 

下面讨论各个函数及其操作：

    main()：主函数创建用来监听连接的套接字，然后创建 accept() 的回调函数以便通过事件处理函数处理每个连接。
    accept_callback()：当接受连接时，事件系统调用此函数。此函数接受到客户端的连接；添加客 户端套接字信息和一个 bufferevent 结构；在事件结构中为客户端套接字上的读/写/错误事件添加回调函数；作为参数传递客户端结构（和嵌入的 eventbuffer 和客户端套接字）。每当对应的客户端套接字包含读、写或错误操作时，调用对应的回调函数。
    buf_read_callback()：当客户端套接字有要读的数据时调用它。作为回显服务，此函数把 "you said..." 写回客户端。套接字仍然打开，可以接受新请求。
    buf_write_callback()：当有要写的数据时调用它。在这个简单的服务中，不需要此函数，所以定义是空的。
    buf_error_callback()：当出现错误时调用它。这包括客户端中断连接。在出现错误的所有场景中，关闭客户端套接字，从事件列表中删除客户端套接字的事件条目，释放客户端结构的内存。
    setnonblock()：设置网络套接字以开放 I/O。 

当客户端连接时，在事件队列中添加新事件以处理客户端连接；当客户端中断连接时删除事件。在幕后，libevent 处理网络套接字，识别需要服务的客户端，分别调用对应的函数。

为了构建这个应用程序，需要编译 C 源代码并添加 libevent 库：$ gcc -o basic basic.c -levent。

从客户端的角度来看，这个服务器仅仅把发送给它的任何文本发送回来（见 清单 3）。


清单 3. 服务器把发送给它的文本发送回来

				
$ telnet localhost 8080
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Hello!
You said Hello!

这样的网络应用程序非常适合需要处理多个连接的大规模分布式部署，比如 IBM Cloud 系统。

很难通过简单的解决方案观察处理大量并发连接的情况和性能改进。可以使用嵌入的 HTTP 实现帮助了解可伸缩性。

使用内置的 HTTP 服务器

如果希望构建本机应用程序，可以使用一般的基于网络的 libevent 接口；但是，越来越常见的场景是开发基于 HTTP 协议的应用程序，以及装载或动态地重新装载信息的网页。如果使用任何 AJAX 库，客户端就需要 HTTP，即使您返回的信息是 XML 或 JSON。

libevent 中的 HTTP 实现并不是 Apache HTTP 服务器的替代品，而是适用于与云和 web 环境相关联的大规模动态内容的实用解决方案。例如，可以在 IBM Cloud 或其他解决方案中部署基于 libevent 的接口。因为可以使用 HTTP 进行通信，服务器可以与其他组件集成。

要想使用 libevent 服务，需要使用与主要网络事件模型相同的基本结构，但是还必须处理网络接口，HTTP 包装器会替您处理。这使整个过程变成四个函数调用（初始化、启动 HTTP 服务器、设置 HTTP 回调函数和进入事件循环），再加上发送回数据的回调函数。清单 4 给出一个非常简单的示例：


清单 4. 使用 libevent 服务的简单示例

				
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

 

应该可以通过前面的示例看出代码的基本结构，不需要解释。主要元素是 evhttp_set_gencb() 函数（它设置当收到 HTTP 请求时要使用的回调函数）和 generic_request_handler() 回调函数本身（它用一个表示成功的简单消息填充响应缓冲区）。

HTTP 包装器提供许多其他功能。例如，有一个请求解析器，它会从典型的请求中提取出查询参数（就像处理 CGI 请求一样）。还可以设置在不同的请求路径中要触发的处理函数。通过设置不同的回调函数和处理函数，可以使用路径 '/db/' 提供到数据库的接口，或使用 '/memc' 提供到 memcached 的接口。

libevent 工具包的另一个特性是支持通用计时器。可以在指定的时间段之后触发事件。可以通过结合使用计时器和 HTTP 实现提供轻量的服务，从而自动地提供文件内容，在修改文件内容时更新返回的数据。例如，以前要想在新闻频发的活动期间提供即时更新服务，前端 web 应用程序就需要定期重新装载新闻稿，而现在可以轻松地提供内容。整个应用程序（和 web 服务）都在内存中，因此响应非常快。

这就是 清单 5 中的示例的主要用途：


清单 5. 使用计时器在新闻频发的活动期间提供即时更新服务

				
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
