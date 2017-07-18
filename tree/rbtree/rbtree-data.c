#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "rbtree.h"


struct commqueue{
	struct rb_node node;
	char sequence[32];
	int sock_from; //bufferevent *bev;
	int commandId;
};

struct rb_root mytree = RB_ROOT;
struct commqueue *search(struct rb_root *root, char *seq)
{
	struct rb_node *node = root->rb_node;

	while (node){
		struct commqueue *data = container_of(node, struct commqueue, node);
		int result;

		result = strcmp(seq, data->sequence);
	
		if (result < 0)
			node = node->rb_left;
		else if (result > 0)
			node = node->rb_right;
		else
			return data;
	}
	return NULL;
}

int insert(struct rb_root *root, struct commqueue *data)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;

	while(*new){
		struct commqueue *this = container_of(*new, struct commqueue, node);
		int result = strcmp(data->sequence, this->sequence);
		
		parent = *new;
		if (result < 0)
			new = &((*new)->rb_left);
		else if (result > 0)
			new = &((*new)->rb_right);
		else
			return 0;
	}
	
	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);

	return 1;
}

void commqueue_free(struct commqueue *node)
{
	if (node != NULL){
		//if (strlen(node->squence != NULL)
		free(node);
		node = NULL;
	}
}

#define NUM_NODES 15

int main()
{
	struct commqueue *mn[NUM_NODES];
	int i = 0;
	printf("insert node from 1 to 32\n");
	for (; i< NUM_NODES; i++){
		mn[i] = (struct commqueue *)malloc(sizeof(struct commqueue));
		sprintf(mn[i]->sequence, "12345678901234567890%d%d", i, i);
		mn[i]->commandId = i;
		mn[i]->sock_from = i;
		insert(&mytree, mn[i]);
	}

	struct rb_node *node;
	printf("search all nodes:\n");
	for (node = rb_first(&mytree); node; node = rb_next(node))
	{
		printf("squence=%s, commandId=%d,sock_from=%d\n",
			rb_entry(node, struct commqueue, node)->sequence,
			rb_entry(node, struct commqueue, node)->commandId,
			rb_entry(node, struct commqueue, node)->sock_from);
	}

	printf("delete node 1234567890123456789022:\n");
	struct commqueue *data = search(&mytree, "1234567890123456789022");
	
	if (data)
	{
		rb_erase(&data->node, &mytree);
		commqueue_free(data);
	}

	
	printf("search all nodes:\n");
	for (node = rb_first(&mytree); node; node = rb_next(node))
	{
		printf("squence=%s, commandId=%d,sock_from=%d\n",
			rb_entry(node, struct commqueue, node)->sequence,
			rb_entry(node, struct commqueue, node)->commandId,
			rb_entry(node, struct commqueue, node)->sock_from);
	}
}
