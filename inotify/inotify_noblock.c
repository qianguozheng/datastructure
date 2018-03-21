#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

int inotify_check(int fd){
	
	int length, i = 0;
	char buffer[BUF_LEN];
	
  length = read( fd, buffer, BUF_LEN );  
 
  if ( length < 0 ) {
    perror( "read" );
    return -1;
  }
 
  while ( i < length ) {
    struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
    if ( event->len ) {
		if (event->mask & IN_CLOSE_WRITE || event->mask & IN_MOVED_TO) {
			if (0 == strcmp(event->name, "magicwifi.host")) {
				printf("magicwifi.host changed\n");
			}
			printf("close write / moved to, filename=%s\n", event->name);
		}
    }
    i += EVENT_SIZE + event->len;
  }
}

int init_inotify(){
	int return_value;
	fd_set descriptors;
	struct timeval time_to_wait;

	int fd, wd;
	int keep_running = 1;

	//fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
	fd = inotify_init();

	if ( fd < 0 ) {
		perror( "inotify_init" );
	}

	wd = inotify_add_watch( fd, "/tmp/", 
						IN_CLOSE_WRITE | IN_MOVED_TO );
						 //IN_MODIFY | IN_CREATE | IN_DELETE );
	printf("fd=%d, wd=%d\n", fd, wd);
	while (keep_running) {
		FD_ZERO ( &descriptors );
		FD_SET ( fd, &descriptors );
		 
		time_to_wait.tv_sec = 3;
		time_to_wait.tv_usec = 0;
		 
		return_value = select ( fd + 1, &descriptors, NULL, NULL, &time_to_wait);
		 
		if ( return_value < 0 ) {
			/* Error */
			printf("error\n");
		}
		 
		else if ( ! return_value ) {
			/* Timeout */
			//printf("timeout\n");
		}
		 
		else if ( FD_ISSET ( fd, &descriptors ) ) {
			/* Process the inotify events */
			inotify_check(fd);
		}
	}
	
	 
  ( void ) inotify_rm_watch( fd, wd );
  ( void ) close( fd );
}

int main( int argc, char **argv ) 
{
	init_inotify();
	return 0;
}

