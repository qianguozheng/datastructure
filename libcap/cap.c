#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
 
#undef _POSIX_SOURCE
#include <sys/capability.h>
 
extern int errno;
  
void whoami(void)
{
  printf("uid=%i  euid=%i  gid=%i\n", getuid(), geteuid(), getgid());
}
 
void listCaps()
{
  cap_t caps = cap_get_proc();
  ssize_t y = 0;
  printf("The process %d was give capabilities %s\n",
         (int) getpid(), cap_to_text(caps, &y));
  fflush(0);
  cap_free(caps);
}
  
int main(int argc, char **argv)
{
  int stat;
  whoami();
  stat = setuid(geteuid());
  pid_t parentPid = getpid();
 
  if(!parentPid)
    return 1;
  cap_t caps = cap_init();
 
 
  cap_value_t capList[5] =
  { CAP_NET_RAW, CAP_NET_BIND_SERVICE , CAP_SETUID, CAP_SETGID,CAP_SETPCAP } ;
  unsigned num_caps = 5;
  cap_set_flag(caps, CAP_EFFECTIVE, num_caps, capList, CAP_SET);
  cap_set_flag(caps, CAP_INHERITABLE, num_caps, capList, CAP_SET);
  cap_set_flag(caps, CAP_PERMITTED, num_caps, capList, CAP_SET);
 
  if (cap_set_proc(caps)) {
    perror("capset()");
 
    return EXIT_FAILURE;
  }
  listCaps();

  printf("dropping caps\n");
  cap_clear(caps);  // resetting caps storage
 
  if (cap_set_proc(caps)) {
    perror("capset()");
    return EXIT_FAILURE;
  }
  listCaps();

  cap_free(caps);
  return 0;

}
