#include <mntent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/vfs.h>

#define FALSE 0
#define TRUE 1

static void printsize(long long n)
{
	char unit = 'K';
	n /= 1024;
	if (n > 1024) {
		n /= 1024;
		unit = 'M';
	}
	if (n > 1024) {
		n /= 1024;
		unit = 'G';
	}
	printf("%4lld%c", n, unit);
}

int main(){
    struct mntent* mnt, *mount_entry;
    FILE* mount_table;

    mount_table = setmntent("/proc/mounts", "r");
    if ( !mount_table )
    {
		printf("mount_table==%p\n", mount_table);
        return FALSE;
    }

	if(mnt=getmntent(mount_table))
	{

	#if 1
		 printf("woosoori[%s:%d] mnt->mnt_fsname=%s\n",__FUNCTION__,__LINE__, mnt->mnt_fsname);
	   printf("woosoori[%s:%d] mnt->mnt_dir=%s\n",__FUNCTION__,__LINE__, mnt->mnt_dir);
	   printf("woosoori[%s:%d]mnt->mnt_type=%s\n",__FUNCTION__,__LINE__,mnt->mnt_type);
	   printf("woosoori[%s:%d] mnt->mnt_opts=%s\n",__FUNCTION__,__LINE__, mnt->mnt_opts);
	   printf("woosoori[%s:%d]mnt->mnt_freq=%d\n",__FUNCTION__,__LINE__,mnt->mnt_freq);   
	   printf("woosoori[%s:%d]mnt->mnt_passno=%d\n",__FUNCTION__,__LINE__,mnt->mnt_passno);   
	#endif
		 //endmntent(mount_table);
		 //return TRUE;
	}
	
	struct statfs st;
	while (1){
			const char *device;
			const char *mount_point;
			const char *fs_type;
			
			if (mount_table){
					mount_entry = getmntent(mount_table);
					if (!mount_entry){
						endmntent(mount_table);
						break;
					}
			} else {
				
			}
			
			device = mount_entry->mnt_fsname;
			mount_point = mount_entry->mnt_dir;
			fs_type = mount_entry->mnt_type;
			
			if (mount_point && !strstr(mount_point, "/mnt")){
					continue;
			}
			printf("device=[%s], mount_point=[%s], fs_type=[%s]\n",
				device, mount_point, fs_type);
			if (statfs(mount_point, &st) != 0){
				printf("statfs error\n");
				return 0;
			}
			printf("%-20s",mount_point);
			printf("%-20s",mount_point);
			printf("\nTotal Size\n");
			printf("f_blocks=%lld, f_bsize=%lld\n", st.f_blocks, st.f_bsize);
			printsize((long long)st.f_blocks*(long long)st.f_bsize);
			printf("\nUsed Size\n");
			printsize((long long)(st.f_blocks - (long long)st.f_bfree) * st.f_bsize);
			printf("\nFree Size\n");
			printsize((long long)st.f_bfree *(long long)st.f_bsize);
			printf("\n");
			printf(" %d\n",(int) st.f_bsize);
	}
}
