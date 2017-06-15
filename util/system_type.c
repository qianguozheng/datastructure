/*************************************************************************
	> File Name: system_type.c
	> Author: 
	> Mail: 
	> Created Time: 2017年04月25日 星期二 10时28分25秒
 ************************************************************************/

#include<stdio.h>


enum {
    AR9344=0,
    AR9341,
    MT7620,
    QCA9531,
    Intel
};

struct modelInfo {
    int index;
    char iface[8];
};

//#define AR9344_B 1
struct modelInfo model[8] = {
#ifdef AR9344_B
 //AR9344-B
 [AR9344] = {AR9344,"eth0"},
#else
 //AR9344-A
 [AR9344] = {AR9344,"eth1"},
#endif
 [AR9341] = {AR9341,"eth0"},
 [MT7620] ={MT7620,"eth0.2"},
 [QCA9531]={QCA9531, "eth0"},
 [Intel]= {Intel,"Intel"}  
 };

#define BUF_LEN 1024

int get_system_type()
{
    FILE *fp=NULL;
    char buf[BUF_LEN];
    int ret = 0;
    
    memset(buf, 0, BUF_LEN);

    if (fp = fopen("/proc/cpuinfo", "r")){
        while (fgets(buf, BUF_LEN, fp)){
            printf("buf=%s\n", buf);
            if (strstr(buf, "AR9344"))
            {
                ret = AR9344;
                break;
            }
            else if (strstr(buf, "AR9341"))
            {
                ret = AR9341;
                break;
            }
            else if (strstr(buf, "MT7620"))
            {
                ret = MT7620;
                break;
            }
            else if (strstr(buf, "QCA9531"))
            {
                ret = QCA9531;
                break;
            }
            else if (strstr(buf, "Intel"))
            {
                ret = Intel;
                printf("ret=%d\n", ret);
                break;
            }
            
            memset(buf, 0, BUF_LEN);
        }
        fclose(fp);
    }

    return ret;
}

int main()
{
    int index;
  index =  get_system_type(); 

    printf("index=%d\n", index);
    struct modelInfo m = model[index];
    printf("iface=%s\n", model[index].iface);
    printf("index=%d\n", model[index].index);

}
