#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <sys/stat.h>
#include <Winsock2.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_FILE "F:\\Libevent\\LibeventTest\\Debug\\sample.txt"

char *filedata;
time_t lasttime = 0;
char filename[80];
int counter = 0;

struct event *loadfile_event;
struct timeval tv;

void read_file()
{
    unsigned long size = 0;
    char *data;
    struct stat buf;

    if(stat(filename,&buf)<0)
    {
        printf("Read file error! \n");
        return;
    }

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
            return;
        }

        size = buf.st_size;
        filedata = (char *)malloc(size+1);
        memset(filedata,0,size+1);
        fread(filedata, sizeof(char), size, f);
        fclose(f);

        fprintf(stderr," (%d bytes)\n",size);
        lasttime = buf.st_mtime;
    }
}

void read_file_timer_cb(evutil_socket_t listener, short event, void *arg)
{
    if (!evtimer_pending(loadfile_event, NULL)) 
    {
        event_del(loadfile_event);
        evtimer_add(loadfile_event, &tv);
    }

    read_file();
}

void load_file(struct event_base * base)
{
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    //loadfile_event = malloc(sizeof(struct event));
    loadfile_event = evtimer_new(base,read_file_timer_cb,NULL);

    //evtimer_set(loadfile_event,load_file,loadfile_event);
    evtimer_add(loadfile_event,&tv);
}

void generic_handler(struct evhttp_request *req, void *arg)
{
    struct evbuffer *buf = evbuffer_new();
    if(!buf)
    {
        puts("failed to create response buffer \n");
        return;
    }
    evbuffer_add_printf(buf,"%s",filedata);
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);
}


int main(int argc, char* argv[])
{

    short          http_port = 8081;
    char          *http_addr = "127.0.0.1";

    struct event_base * base = event_base_new();

    struct evhttp * http_server = evhttp_new(base);
    if(!http_server)
    {
        return -1;
    }

    int ret = evhttp_bind_socket(http_server,http_addr,http_port);
    if(ret!=0)
    {
        return -1;
    }

    evhttp_set_gencb(http_server, generic_handler, NULL);

    //read_file();

    //load_file(base);

    printf("http server start OK! \n");

    event_base_dispatch(base);

    evhttp_free(http_server);


    return 0;
}
