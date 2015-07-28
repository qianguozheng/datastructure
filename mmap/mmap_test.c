/*************************************************************************
	> File Name: mmap_test.c
	> Author: qianguozheng
	> Mail: guozhengqian0825@126.com
	> Created Time: Thu Jul 23 14:46:56 2015
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>

//定义存放记录的结构体

typedef struct
{
    int index;
    char text[10];
} RECORD;

#define SIZE (50)
#define EDIT_INDEX (10)

int main(void)
{
    RECORD record, *p_mapped_memory_addr;
    int i, fd;
    FILE *fp;

    //创建文件并写入测试数据
    fp = fopen("records.dat", "w+");
    for (i = 0; i < SIZE; i++)
    {
        record.index = i;
        sprintf(record.text, "No.%d", i);
        fwrite(&record, sizeof(record), 1, fp);
    }
    fclose(fp);
    
    printf("OK, write %d records to the file: records.data.\n", SIZE);

    //将第一30条记录编号修改为300, 并相应的修改内容。
    //采用传统方式
    
    fp = fopen("records.dat", "r+");
    fseek(fp, EDIT_INDEX * sizeof(record), SEEK_SET);
    fread(&record, sizeof(record), 1, fp);

    record.index = EDIT_INDEX * 10;
    sprintf(record.text, "No.%d", record.index);

    fseek(fp, EDIT_INDEX * sizeof(record), SEEK_SET);
    fwrite(&record, sizeof(record), 1, fp);
    fclose(fp);

    printf("OK, edit the file of records.dat using traditional method.\n");

    //同样的修改，这次使用内存映射的方式。
    //将记录映射到内存中。
    
    fd = open("records.dat", O_RDWR);
    p_mapped_memory_addr = (RECORD *) mmap(0, SIZE * sizeof(record), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    //修改数据
    p_mapped_memory_addr[EDIT_INDEX].index = EDIT_INDEX * 10;
    sprintf(p_mapped_memory_addr[EDIT_INDEX].text, "No.%d", p_mapped_memory_addr[EDIT_INDEX].index);

    //将修改写回映射文件中（采用异步写方式）
    msync((void *)p_mapped_memory_addr, SIZE*sizeof(record), MS_ASYNC);

    munmap((void *)p_mapped_memory_addr, SIZE *sizeof(record));
    printf("OK, edit the file of records.dat using mmap method.\n");

    close(fd);

    return 0;
}
