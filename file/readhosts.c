#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

static int eatspace(FILE *f)
{
  int c, nl = 0;

	while (1)
    {
		if ((c = getc(f)) == '#')
			while (c != '\n' && c != EOF)
				c = getc(f);
		  
		if (c == EOF)
			return 1;

		if (!isspace(c))
		{
			ungetc(c, f);
			return nl;
		}

		if (c == '\n')
			nl = 1;
    }
}

#define MAXDNAME	1025		/* maximum presentation domain name */
static int gettok(FILE *f, char *token)
{
  int c, count = 0;
 
	while (1)
    {
		if ((c = getc(f)) == EOF)
			return (count == 0) ? EOF : 1;

		if (isspace(c) || c == '#')
		{
			ungetc(c, f);
			return eatspace(f);
		}
		  
		if (count < (MAXDNAME - 1))
		{
			token[count++] = c;
			token[count] = 0;
		}
    }
}

static int read_hostsfile(char *filename)
{  
	FILE *f = fopen(filename, "r");
	char token[MAXDNAME];
	int addr_count = 0, lineno = 0;
	unsigned short flags = 0;
	int atnl, addrlen = 0;

	memset(token, 0, sizeof(token));
	if (!f)
    {
		printf("failed to load names from %s: %s", filename, strerror(errno));
		return 0;
    }
  
	eatspace(f);
	
	while ((atnl = gettok(f, token)) != EOF)
	{
		printf("token=%s\n", token);
		lineno++;
      
      addr_count++;
      
		while (atnl == 0)
		{
		  int fqdn, nomem;
		  char *canon;
		  
		  if ((atnl = gettok(f, token)) == EOF)
			break;

		  fqdn = !!strchr(token, '.');
		}

    }

  fclose(f);
  //rehash(name_count);
  
  printf("read %s - %d addresses", filename, addr_count);
  
  return 0;
}

int get_file_size(char *filename)
{
	struct stat buf;
	stat(filename, &buf);
	return buf.st_size;
}

int modify_hostsfile(char *filename, char *ip, const char *domain){
	int size = get_file_size(filename);
	int buff_len = size + 16 + MAXDNAME;
	char *buff = malloc(buff_len), line[1024], desired_domain[1024];
	char *ptr = NULL;
	int len = 0, found = 0;
	
	memset(buff, 0, buff_len);
	memset(line, 0, sizeof(line));
	memset(desired_domain, 0, sizeof(desired_domain));
	
	sprintf(desired_domain, " %s", domain);
	
	FILE *fp = fopen(filename, "r+");
	if (!fp){
		printf("open file failed %s\n", filename);
	}
	
	while(!feof(fp) && fgets(line, sizeof(line)-1, fp)){
		printf("line=%s\n", line);
		if ((ptr = strstr(line, desired_domain)) != NULL
			&& ( *(ptr+strlen(desired_domain)) == ' '
				|| *(ptr+strlen(desired_domain)) == '\n'
				|| *(ptr+strlen(desired_domain)) == '\r'
				|| *(ptr+strlen(desired_domain)) == '#'
				|| *(ptr+strlen(desired_domain)) == EOF
			)){
			//printf("line=%s\n", line);
			memset(line, 0, sizeof(line));
			sprintf(line, "%s %s\n", ip, domain);
			found = 1;
		}
		
		memcpy(buff+len, line, strlen(line));
		len += strlen(line);
	}
	if (0  == found){
		memset(line, 0, sizeof(line));
		sprintf(line, "%s %s\n", ip, domain);
		memcpy(buff+len, line, strlen(line));
		len += strlen(line);
	}
	//printf("rewind the fp\n");
	fseek(fp, 0, SEEK_SET);
	fwrite(buff, len, 1, fp);
	fflush(fp);
	fclose(fp);
}

int main(){
	//read_hostsfile("/etc/hosts");
	modify_hostsfile("hosts", "192.168.312.136", "r.magicwifi.com.cn");
}
