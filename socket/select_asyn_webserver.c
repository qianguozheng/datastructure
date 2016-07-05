#include <sys/socket.h>
#include <string.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>

#define TRUE 1
#define FALSE 0

#define RESPONSE_OK "HTTP/1.1 200 OK\r\n" \
					"Connection: close\r\n\r\n"
int main(int argc, char *argv[])
{
	int i, len, rc, on = TRUE;
	int listen_fd, new_fd =0, max_sd;
	int desc_ready;
	char buffer[80];
	int close_conn, end_server = FALSE;
	struct sockaddr_in server_addr;
	struct timeval timeout;
	fd_set master_set, working_set;
	
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
	{
		perror("socket");
		exit(-1);
	}
	
	rc = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt");
		close(listen_fd);
		exit(-1);
	}
	
	rc = ioctl(listen_fd, FIONBIO, (char *)&on);
	if (rc < 0)
	{
		perror("ioctl");
		close(listen_fd);
		exit(-1);
	}
	
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	rc = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (rc < 0)
	{
		perror("bind");
		close(listen_fd);
		exit(-1);
	}
	
	rc = listen(listen_fd, 32);
	if (rc < 0)
	{
		perror("listen()");;
		close(listen_fd);
		exit(-1);
	}
	
	//Intialize fd set
	FD_ZERO(&master_set);
	max_sd = listen_fd;
	FD_SET(listen_fd, &master_set);
	
	timeout.tv_sec = 3 * 60;
	timeout.tv_usec = 0;
	
	do 
	{
		printf("copy file descriptor\n");
		memcpy(&working_set, &master_set, sizeof(master_set));
		printf("Waiting on select()...\n");
		rc = select(max_sd + 1, &working_set, NULL, NULL, &timeout);
		if (rc < 0)
		{
			perror("select");
			break;
		}
		if (rc == 0)
		{
			printf("select timeout ");
			break;
		}
		desc_ready = rc;
		//check each sd in working_set
		for (i=0; i<= max_sd && desc_ready > 0; ++i)
		{
			if (FD_ISSET(i, &working_set))
			{
				--desc_ready;
				if (i==listen_fd)
				{
					printf("Listening socket is readable\n");
					do 
					{
						new_fd = accept(listen_fd, NULL, NULL);
						
						if (new_fd < 0)
						{
							if (errno != EWOULDBLOCK)
							{
								perror("accept====");
								end_server = TRUE;
							}
							//perror("accept");
							continue;
						}
						//Insert new_fd into master_set
						printf("New incoming connections - %d\n", new_fd);
						FD_SET(new_fd, &master_set);
						if (new_fd > max_sd)
						{
							max_sd = new_fd;
						}
						printf("continue\n");
						//continue;
					}
					while(new_fd != -1);
				}
				else
				{
					close_conn = FALSE;
					printf("Descriptor %d is avaliable\n", i);
					
					do {
						rc = recv(i, buffer, sizeof(buffer), 0);
						if (rc < 0)
						{
							if (errno != EWOULDBLOCK)
							{
								perror("recv");
								close_conn = TRUE;
							}
							break;
						}
						if (0 == rc)
						{
							printf("Connection closed\n");
							close_conn = TRUE;
							break;
						}
						len = rc;
						printf("  %d bytes received \n", len);
						
						memset(buffer, 0, sizeof(buffer));
						memcpy(buffer, RESPONSE_OK, strlen(RESPONSE_OK));
						rc = send(i, buffer, strlen(buffer), 0);
						if (rc < 0)
						{
							perror("send");
							close_conn = TRUE;
							break;
						}
						else
						{
							printf("send %d bytes\n", rc);
							//close(i);
							//close_conn = TRUE;
							break;
						}
						
						//close(i);
					} while(TRUE);
					
					if (close_conn)
					{
						close(i);
						FD_CLR(i, &master_set);
						
						if (i == max_sd)
						{
							while(FD_ISSET(max_sd, &master_set) == FALSE)
							{
								--max_sd;
							}
						}
					}
				}
			}
			printf("FD_ISSET not notifyed\n");
		}
		printf("Big while, end_server=%d\n", end_server);
	}
	while (end_server == FALSE);
	
	printf("End server\n");
	for (i = 0; i < max_sd; i++)
	{
		if(FD_ISSET(i, &master_set))
		{
			close(i);
		}
	}
	
	return 0;
}


