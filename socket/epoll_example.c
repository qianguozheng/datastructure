#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>

//http://stackoverflow.com/questions/13615233/c-epoll-port-forwarding-proxy-doesnt-forward-all-data

#define FALSE       0
#define TRUE        1
#define EPOLL_QUEUE_LEN 256
#define SERVER_PORT 7000
#define LISTEN_PORT  8667       // Proxy Server Listens for this port
#define FORWARD_PORT 8888       // Proxy Server forwards all port LISTEN_PORT data to this port
#define BUFLEN 1024
//Globals
int fd_server;
int sent = 0; // for how many requests were processed to client
int forwardSockets[EPOLL_QUEUE_LEN];
int internalSockets[EPOLL_QUEUE_LEN];
// Function prototypes
static void SystemFatal (const char* message);
static int forwardData (int fd);
void close_fd (int);

// This is the main function which handles the epoll loop and 
// accepts connections
// Also handles the data processing back to the client.
int main (int argc, char* argv[]) 
{
    int i, arg, src_port, forwardSD, dest_port;
    int num_fds, fd_new, epoll_fd;
    int linenum=0;
    static struct epoll_event events[EPOLL_QUEUE_LEN], event;
    struct sockaddr_in addr, remote_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    struct sigaction act;
    struct  hostent *hp;
    struct  sockaddr_in server_fwd;
    char *host;
        char line[256], ip[256];
    FILE *fp;

    fp=fopen("port_forward_config", "r");
    //src_port = LISTEN_PORT;           // Use the default listen port


        while(fgets(line, 256, fp) != NULL)
        {
                linenum++;
                if(line[0] == '#') continue;

                sscanf(line, "%s %d %d", &ip, &src_port, &dest_port);
        {
                    fprintf(stderr, "Syntax error, line %d\n", linenum);
                continue;
        }
    }
    printf("Reading Config File...\n");
    printf("IP %s LPORT %d DPORT %d\n", ip, src_port, dest_port);
    host = ip;

    // set up the signal handler to close the server socket when CTRL-c is received
    act.sa_handler = close_fd;
    act.sa_flags = 0;
    if ((sigemptyset (&act.sa_mask) == -1 || sigaction (SIGINT, &act, NULL) == -1))
    {
            perror ("Failed to set SIGINT handler");
            exit (EXIT_FAILURE);
    }
//--------------------------------------------------------------------------------------------  
    // Create the listening socket
    fd_server = socket (AF_INET, SOCK_STREAM, 0);
    if (fd_server == -1) 
    {   
        SystemFatal("socket");
    }
    // set SO_REUSEADDR so port can be resused imemediately after exit, i.e., after CTRL-c
    arg = 1;
    if (setsockopt (fd_server, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg)) == -1) 
    {   
        SystemFatal("setsockopt");
    }
    // Make the server listening socket non-blocking
    if (fcntl (fd_server, F_SETFL, O_NONBLOCK | fcntl (fd_server, F_GETFL, 0)) == -1) 
    {   
        SystemFatal("fcntl");
    }
    // Bind to the specified listening port
    memset (&addr, 0, sizeof (struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(src_port);
    if (bind (fd_server, (struct sockaddr*) &addr, sizeof(addr)) == -1) 
    {   
        SystemFatal("bind");
    }
    // Listen for fd_news; SOMAXCONN is 128 by default
    if (listen (fd_server, SOMAXCONN) == -1) 
    {   
        SystemFatal("listen");
    }
//--------------------------------------------------------------------------------------------- 
    // Create the epoll file descriptor
    epoll_fd = epoll_create(EPOLL_QUEUE_LEN);
    if (epoll_fd == -1) 
    {   
        SystemFatal("epoll_create");
    }
    // Add the server socket to the epoll event loop
    event.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET;
    event.data.fd = fd_server;
    if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, fd_server, &event) == -1) 
    {
        SystemFatal("epoll_ctl");
    }   
    // Execute the epoll event loop
    while (TRUE) 
    {
        //struct epoll_event events[MAX_EVENTS];
        num_fds = epoll_wait (epoll_fd, events, EPOLL_QUEUE_LEN, -1);
        if (num_fds < 0) 
        {
            SystemFatal ("Error in epoll_wait!");
        }
        for (i = 0; i < num_fds; i++) 
        {
                // Case 1: Error condition
                if (events[i].events & (EPOLLHUP | EPOLLERR)) 
            {
                    fputs("epoll: EPOLLERR", stderr);
                    //close(events[i].data.fd);
                    continue;
                }
                assert (events[i].events & EPOLLIN);
//-----------------------------------------------------------------------------------------
                // Case 2: Server is receiving a connection request
                if (events[i].data.fd == fd_server) 
            {
                //socklen_t addr_size = sizeof(remote_addr);
                fd_new = accept (fd_server, (struct sockaddr*) &remote_addr, &addr_size);
                if (fd_new == -1) 
                {
                        if (errno != EAGAIN && errno != EWOULDBLOCK) 
                    {
                        perror("accept");
                        }
                        continue;
                }
//------------------------------------------------------------------------------------------------
                // Make the fd_new non-blocking
                if (fcntl (fd_new, F_SETFL, O_NONBLOCK | fcntl(fd_new, F_GETFL, 0)) == -1) 
                {
                    SystemFatal("fcntl");
                }

                // Add the new socket descriptor to the epoll loop
                event.data.fd = fd_new;
                if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, fd_new, &event) == -1) 
                {   
                    SystemFatal ("epoll_ctl");
                }
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                printf(" Remote Address:  %s\n", inet_ntoa(remote_addr.sin_addr));
                //close(fd_new);
                dest_port = FORWARD_PORT;       // Used the default forward port
                // create internal connection
                printf("Trying to create forward socket\n");        
                if ((forwardSD = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                    perror("Cannot create forward socket.");
                    exit(1);
                }

                printf("Binding...\n");
                bzero((char *)&server_fwd, sizeof(struct sockaddr_in));
                server_fwd.sin_family = AF_INET;
                server_fwd.sin_port = htons(dest_port);
                //host = "192.168.0.10";    
                if ((hp = gethostbyname(host)) == NULL) {
                    printf("Failed to get host name");
                }

                bcopy(hp->h_addr, (char *)&server_fwd.sin_addr, hp->h_length);

                printf("Connecting to interal machine.\n");
                printf("Server Forward Port: %d\n", ntohs(server_fwd.sin_port));
                printf("Server Forward IP: %s\n", inet_ntoa(server_fwd.sin_addr));

                // Connecting to interal machine
                if (connect (forwardSD, (struct sockaddr *)&server_fwd, sizeof(server_fwd)) == -1) {
                    perror("connect failed");
                    exit(1);
                }

                // Add the new socket descriptor to the epoll loop
                event.data.fd = forwardSD;
                if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, forwardSD, &event) == -1) 
                {   
                    SystemFatal ("epoll_ctl");
                }   
                printf ("Connected: Server: %s\n", hp->h_name);
                forwardSockets[fd_new] = forwardSD;
                internalSockets[forwardSD] = fd_new;
            // end internal connection
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                continue;
                }
                // Case 3: One of the sockets has read data
                if (!forwardData(events[i].data.fd)) 
            {
                // epoll will remove the fd from its set
                // automatically when the fd is closed
                close (events[i].data.fd);      
            }
        }
    }
    close(fd_server);
    exit (EXIT_SUCCESS);
}

/*
    This function clears a socket if it has data waiting to be processed.
    It takes the string sent from the client and parses it. After it has 
    done that it will send back to the client the amount of data it requested
    for however many requests it defined as well.
*/
static int forwardData (int fd) 
{
    int n, bytes_to_read;
    char    *bp, buf[BUFLEN];
    int     forwardData;
    printf ("Forwarding :\n");

    //check if internal or external connection to send data back to.
    if(forwardSockets[fd] != 0){
        forwardData = forwardSockets[fd];
    }
    if(internalSockets[fd] != 0){
        forwardData = internalSockets[fd];
    }

    bp = buf;
        bytes_to_read = BUFLEN;
        while ((n = recv (fd, bp, bytes_to_read, 0)) > 0) {
                bp += n;
                bytes_to_read -= n;
        send (forwardData, buf, n, 0);
        //return TRUE;
        }
    return TRUE;

}

// Prints the error stored in errno and aborts the program.
static void SystemFatal(const char* message) 
{
    perror (message);
    exit (EXIT_FAILURE);
}
// close fd
void close_fd (int signo)
{
        close(fd_server);
    exit (EXIT_SUCCESS);
}
