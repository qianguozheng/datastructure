#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "rbtree.h"

struct timerqueue_node{
	struct rb_node node;
	time_t expires;
};

struct timerqueue_head{
	struct rb_root head;
	struct timerqueue_node *next;
};

void timerqueue_add(struct timerqueue_head *head, struct timerqueue_node *node);
void timerqueue_del(struct timerqueue_head *head, struct timerqueue_node *node);
struct timerqueue * timerqueue_iterate_next(struct timerqueue_node *node);

static inline
struct timerqueue_node *timerqueue_getnext(struct timerqueue_head *head)
{
	return head->next;
}

static inline void timerqueue_init(struct timerqueue_node *node)
{
	RB_CLEAR_NODE(&node->node);
}

static inline void timerqueue_init_head(struct timerqueue_head *head)
{
	head->head = RB_ROOT;
	head->next = NULL;
}

void timerqueue_add(struct timerqueue_head *head, struct timerqueue_node *node)
{
	struct rb_node **p = &head->head.rb_node;
	struct rb_node *parent = NULL;
	struct timerqueue_node *ptr;

	WARN_ON_ONCE(!RB_EMPTY_NODE(&node->node));

	while(*p){
		parent = *p;
		ptr = rb_entry(parent, struct timerqueue_node, node);
		if (node->expires < ptr->expires)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}

	rb_link_node(&node->node, parent, p);
	rb_insert_color(&node->node, &head->head);

	if (!head->next || node->expires < head->next->expires)
		head->next = node;
}

void timerqueue_del(struct timerqueue_head *head, struct timerqueue_node *node)
{
	WARN_ON_ONCE(RB_EMPTY_NODE(&node->node));

	if(head->next == node){
		struct rb_node *rbn = rb_next(&node->node);

		head->next = rbn ? rb_entry(rbn, struct timerqueue_node, node): NULL;
	}
	rb_erase(&node->node, &head->head);
	RB_CLEAR_NODE(&node->node);
}

struct timerqueue_node *timerqueue_iterate_next(struct timerqueue_node *node)
{
	struct rb_node *next;
	if (!node)
		return NULL;
	next = rb_next(&node->node);
	if (!next)
		return NULL;

	return container_of(next, struct timerqueue_node, node);
}


struct timerqueue_head timerqueue;

typedef struct {
	struct timerqueue_node timer;
	unsigned int ip_suff;
	char cip[16];
	time_t last_updated;
	time_t valid_time;
}IPTM_NODE;

#define TMQ_OUT 120

int timerqueue_append(const char *ip, time_t valid)
{
	unsigned int sip;
	time_t curr;

	IPTM_NODE *inode = (IPTM_NODE*)calloc(1, sizeof(IPTM_NODE));

	timerqueue_init(&inode->timer);
	inode->timer.expires = curr + TMQ_OUT;
	inode->last_updated = curr;
	inode->valid_time = valid;

	sip = 100;
	inode->ip_suff = 1000;
	strncpy(inode->cip, "TEST", sizeof(inode->cip)-1)
	
	timerqueue_add(&timerqueue, &inode->timer);

	return 0;
}

void timerqueue_delete(void *ptr)
{
	IPTM_NODE *inode = (IPTM_NODE*)ptr;
	timerqueue_del(&timerqueue, &inode->timer);
	free(inode);
}


static int checkinterval = 5;
static int clienttimeout = 120;

void timerqueue_process(void)
{
	struct timerqueue_node *node;
	IPTM_NODE *inode;
	
}

