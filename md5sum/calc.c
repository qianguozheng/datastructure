#include "md5.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define READ_DATA_SIZE	1024
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)

int Compute_file_md5(const char *file_path, char *value);

int main(int argc, char *argv[])
{
  int ret;
  const char *file_path = "test";
  char md5_str[MD5_STR_LEN + 1];

  ret = Compute_file_md5(file_path, md5_str);
  if (0 == ret)
  {
    printf("[file - %s] md5 value:\n", file_path);
    printf("%s\n", md5_str);
  }

  return 0;
}

int Compute_file_md5(const char *file_path, char *md5_str)
{
  int i;
  int fd;
  int ret;
  unsigned char data[READ_DATA_SIZE];
  unsigned char md5_value[MD5_SIZE];
  MD5_CTX md5;

  fd = open(file_path, O_RDONLY);
  if (-1 == fd)
  {
    perror("open");
    return -1;
  }

  // init md5
  MD5Init(&md5);

  while (1)
  {
    ret = read(fd, data, READ_DATA_SIZE);
    if (-1 == ret)
    {
      perror("read");
      return -1;
    }

    MD5Update(&md5, data, ret);

    if (0 == ret || ret < READ_DATA_SIZE)
    {
      break;
    }
  }

  close(fd);

  MD5Final(&md5, md5_value);

  for(i = 0; i < MD5_SIZE; i++)
  {
    snprintf(md5_str + i*2, 2+1, "%02x", md5_value[i]);
  }
  md5_str[MD5_STR_LEN] = '\0'; // add end

  return 0;
}
