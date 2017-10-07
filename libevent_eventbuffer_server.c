libevent_eventbuffer_server.c

    #include <netinet/in.h>  
    #include <sys/socket.h>  
    #include <fcntl.h>  
      
    #include <event2/event.h>  
    #include <event2/buffer.h>  
    #include <event2/bufferevent.h>  
      
    #include <assert.h>  
    #include <unistd.h>  
    #include <string.h>  
    #include <stdlib.h>  
    #include <stdio.h>  
    #include <errno.h>  
      
    void do_read(evutil_socket_t fd, short events, void *arg);  
      
    //struct bufferevent�ڽ�������event(read/write)�Ͷ�Ӧ�Ļ�����(struct evbuffer *input, *output)�����ṩ��Ӧ�ĺ�����������>  
    ������(����ֱ�Ӳ���bufferevent)  
    //���յ����ݺ��ж��ǲ�һ��һ����Ϣ�Ľ�����������־Ϊ"over"�ַ���  
    void readcb(struct bufferevent *bev, void *ctx)  
    {  
        printf("called readcb!\n");  
        struct evbuffer *input, *output;  
        char *request_line;  
        size_t len;  
        input = bufferevent_get_input(bev);//��ʵ����ȡ��bufferevent�е�input  
        output = bufferevent_get_output(bev);//��ʵ����ȡ��bufferevent�е�output  
      
        size_t input_len = evbuffer_get_length(input);  
        printf("input_len: %d\n", input_len);  
        size_t output_len = evbuffer_get_length(output);  
        printf("output_len: %d\n", output_len);  
      
        while(1)  
        {  
            request_line = evbuffer_readln(input, &len, EVBUFFER_EOL_CRLF);//��evbufferǰ��ȡ��һ�У���һ���·���Ŀ��ַ�����  
    ���ַ���������һ��,EVBUFFER_EOL_CRLF��ʾ��β��һ����ѡ�Ļس�������һ�����з�  
            if(NULL == request_line)  
            {  
                printf("The first line has not arrived yet.\n");  
                free(request_line);//֮����Ҫ����free����Ϊ line = mm_malloc(n_to_copy+1))�������������malloc  
                break;  
            }  
            else  
    <span style="white-space: pre;">    </span>{  
                printf("Get one line date: %s\n", request_line);  
                if(strstr(request_line, "over") != NULL)//�����ж��ǲ���һ����Ϣ�Ľ���  
                {  
                    char *response = "Response ok! Hello Client!\r\n";  
                    evbuffer_add(output, response, strlen(response));//Adds data to an event buffer  
                    printf("����˽���һ��������ɣ��ظ��ͻ���һ����Ϣ: %s\n", response);  
                    free(request_line);  
                    break;  
                }  
            }  
            free(request_line);  
        }  
      
        size_t input_len1 = evbuffer_get_length(input);  
        printf("input_len1: %d\n", input_len1);  
        size_t output_len1 = evbuffer_get_length(output);  
        printf("output_len1: %d\n\n", output_len1);  
    }  
      
    void errorcb(struct bufferevent *bev, short error, void *ctx)  
    {  
        if (error & BEV_EVENT_EOF)  
        {  
            /* connection has been closed, do any clean up here */  
            printf("connection closed\n");  
        }  
        else if (error & BEV_EVENT_ERROR)  
        {  
            /* check errno to see what error occurred */  
            printf("some other error\n");  
        }  
        else if (error & BEV_EVENT_TIMEOUT)  
        {  
            /* must be a timeout event handle, handle it */  
            printf("Timed out\n");  
        }  
        bufferevent_free(bev);  
    }  
      
    void do_accept(evutil_socket_t listener, short event, void *arg)  
    {  
        struct event_base *base = arg;  
        struct sockaddr_storage ss;  
        socklen_t slen = sizeof(ss);  
        int fd = accept(listener, (struct sockaddr*)&ss, &slen);  
        if (fd < 0)  
        {  
            perror("accept");  
        }  
        else if (fd > FD_SETSIZE)  
        {  
            close(fd);  
        }  
        else  
        {  
            struct bufferevent *bev;  
            evutil_make_socket_nonblocking(fd);  
      
            //ʹ��bufferevent_socket_new����һ��struct bufferevent *bev��������sockfd���йܸ�event_base  
            ////BEV_OPT_CLOSE_ON_FREE��ʾ�ͷ�buffereventʱ�رյײ㴫��˿ڡ��⽫�رյײ��׽��֣��ͷŵײ�bufferevent�ȡ�  
            bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);  
      
            //���ö�д��Ӧ�Ļص�����  
            bufferevent_setcb(bev, readcb, NULL, errorcb, NULL);  
    //      bufferevent_setwatermark(bev, EV_READ, 0, MAX_LINE);  
      
            //���ö�д�¼�,��ʵ�ǵ�����event_add����Ӧ��д�¼������¼���������poll�������ĵ���˵�������Ӧ�¼�����Ϊtrue��buf  
    ferevent�ǲ����д���ݵ�  
            bufferevent_enable(bev, EV_READ|EV_WRITE);  
        }  
    }  
      
    void run(void)  
    {  
        evutil_socket_t listener;  
        struct sockaddr_in sin;  
        struct event_base *base;  
        struct event *listener_event;  
      
        base = event_base_new();  
        if (!base)  
            return; /*XXXerr*/  
      
        sin.sin_family = AF_INET;  
        sin.sin_addr.s_addr = 0;  
        sin.sin_port = htons(8000);  
      
        listener = socket(AF_INET, SOCK_STREAM, 0);  
        evutil_make_socket_nonblocking(listener);  
      
    #ifndef WIN32  
        {  
            int one = 1;  
            setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));  
        }  
    #endif  
      
        if (bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0)  
        {  
            perror("bind");  
            return;  
        }  
    if (listen(listener, 16)<0)  
        {  
            perror("listen");  
            return;  
        }  
      
        listener_event = event_new(base, listener, EV_READ|EV_PERSIST, do_accept, (void*)base);  
        /*XXX check it */  
        event_add(listener_event, NULL);  
      
        event_base_dispatch(base);  
    }  
      
    int main(int argc, char **argv)  
    {  
        setvbuf(stdout, NULL, _IONBF, 0);  
      
        run();  
        return 0;  
    }  

���룺gcc -I/usr/include-o test libevent_eventbuffer_server.c -L/usr/local/lib �Clevent