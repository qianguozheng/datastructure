#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

int dnsquery(const char *host, int portnum, char *ip_addr)
{
   struct addrinfo hints, *result, *rp;
   char service[6];
   int retval;
   int fd;
   fd_set wfds;
   struct timeval timeout;

   int connect_failed;
   /*
    * XXX: Initializeing it here is only necessary
    *      because not all situations are properly
    *      covered yet.
    */
   int socket_error = 0;

	sprintf(service, "%d", portnum);
   memset((char *)&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_NUMERICSERV; /* avoid service look-up */
#ifdef AI_ADDRCONFIG
   hints.ai_flags |= AI_ADDRCONFIG;
#endif

   if ((retval = getaddrinfo(host, service, &hints, &result)))
   {
	  printf("Can not resolve %s: %s", host, gai_strerror(retval));
      /* XXX: Should find a better way to propagate this error. */
      errno = EINVAL;

      return(-1);
   }

   for (rp = result; rp != NULL; rp = rp->ai_next)
   {

      retval = getnameinfo(rp->ai_addr, rp->ai_addrlen,
         ip_addr, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
      if (retval)
      {
         printf("Failed to get the host name from the socket structure: %s", gai_strerror(retval));
         continue;
      }

	printf("Address=%s", ip_addr);
	if (0 == strcasecmp("::", ip_addr) ||
		0 == strcasecmp("::1", ip_addr))
	{
		printf("Address=%s, Local IP Address", ip_addr);
		continue;
	}

	//---- Connect to socket ----//
#if 0
      fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

      if (fd < 0)
      {
         continue;
      }


      set_no_delay_flag(fd);

      if ((flags = fcntl(fd, F_GETFL, 0)) != -1)
      {
         flags |= O_NDELAY;
         fcntl(fd, F_SETFL, flags);
      }


      connect_failed = 0;
      while (connect(fd, rp->ai_addr, rp->ai_addrlen) == INVALID_SOCKET)
      {
         if (errno == EINPROGRESS)
         {
            break;
         }

         if (errno != EINTR)
         {
            socket_error = errno;
            close(fd);
            connect_failed = 1;
            break;
         }
      }
      if (connect_failed)
      {
         continue;
      }


      if (flags != -1)
      {
         flags &= ~O_NDELAY;
         fcntl(fd, F_SETFL, flags);
      }

      /* wait for connection to complete */
      FD_ZERO(&wfds);
      FD_SET(fd, &wfds);

      memset(&timeout, 0, sizeof(timeout));
      timeout.tv_sec  = 30;

      /* MS Windows uses int, not SOCKET, for the 1st arg of select(). Weird! */
      if ((select((int)fd + 1, NULL, &wfds, NULL, &timeout) > 0)
         && FD_ISSET(fd, &wfds))
      {
         socklen_t optlen = sizeof(socket_error);
         if (!getsockopt(fd, SOL_SOCKET, SO_ERROR, &socket_error, &optlen))
         {
            if (!socket_error)
            {
               /* Connection established, no need to try other addresses. */
               break;
            }
            if (rp->ai_next != NULL)
            {
               /*
                * There's another address we can try, so log that this
                * one didn't work out. If the last one fails, too,
                * it will get logged outside the loop body so we don't
                * have to mention it here.
                */
               printf("Could not connect to [%s]:%s: %s.", ip_addr, service, strerror(socket_error));
            }
         }
         else
         {
            socket_error = errno;
            printf("Could not get the state of "
               "the connection to [%s]:%s: %s; dropping connection.",
               ip_addr, service, strerror(errno));
         }
      }

      /* Connection failed, try next address */
      close(fd);
#endif

   }



   freeaddrinfo(result);
   
#if 0
   if (!rp)
   {
      printf("Could not connect to [%s]:%s:", host, service);
      return(-1);
   }
#endif
   
   printf("Connected to %s[%s]:%s.", host, ip_addr, service);
   return(fd);

}

void main(int argc, char *argv[]){
	char ip[16];
	memset(ip , 0, sizeof(ip));
	dnsquery("www.hiweeds.net", 80, ip);
}
