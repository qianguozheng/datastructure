#include "gap_mclist.h"
#include <stdio.h>

typedef struct numlist{
		int num;
		struct list_head list;
	} NUM;
int main(void)
{
	NUM numhead;
	NUM *listnode;
	struct list_head * pos;
	NUM *p;
	int i;
	
	INIT_LIST_HEAD(&numhead.list);
	for (i = 0; i < 10; i++)
	{
		listnode =  (NUM *) malloc(sizeof(NUM));
		listnode->num = i+1;
		list_add_tail(&listnode->list, &numhead.list);
		printf("Node %d has added to the doublelist...\n", i+1);
	}
	
	i = 1;
	list_for_each(pos, &numhead.list){
		p = list_entry(pos, NUM, list);
		printf("Node %d's data: %d\n", i, p->num);
		i++;
	}
	
	i = 0;
	struct list_head *n;
	list_for_each_safe(pos, n, &numhead.list){
		//For safe delete node 
		if (pos->num == 4)
		{
			list_del(pos);
			p = list_entry(pos, NUM, list);
			free(p);
		//}
			printf("Nod %d has removed from the doublelist...\n", i++);
		}
	}
	
	//list_del();
	
	return 0;

}
