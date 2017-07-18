#include <stdio.h>
#include <stdlib.h>
typedef struct Node{
	struct Node *l;
	struct Node *r;
	struct Node *p;
	int v;
}Node;
Node *root;
void rightRotate(Node *P)
{
	Node *T=P->l;
	Node *B=T->r;
	Node *D=P->p;
	if(D)
	{
		if(D->r==P) D->r=T;
		else D->l=T;
	}
	if(B)
		B->p=P;
	T->p=D;
	T->r=P;
	
	P->p=T;
	P->l=B;
}
void leftRotate(Node *P)
{
	Node *T=P->r;
	Node *B=T->l;
	Node *D=P->p;
	if(D)
	{
		if(D->r==P) D->r=T;
		else D->l=T;
	}
	if(B)
		B->p=P;
	T->p=D;
	T->l=P;
	
	P->p=T;
	P->r=B;
}

void Splay(Node *T)
{
	while(1)
	{
		Node *p=T->p;
		if(!p) break;
		Node *pp=p->p;
		if(!pp)//Zig
		{
			if(p->l==T)
				rightRotate(p);
			else
				leftRotate(p);
			break;
		}
		if(pp->l==p)
		{
			if(p->l==T)
			{//ZigZig
				rightRotate(pp);
				rightRotate(p);
			}
			else
			{//ZigZag
				leftRotate(p);
				rightRotate(pp);
			}
		}
		else
		{
			if(p->l==T)
			{//ZigZag
				rightRotate(p);
				leftRotate(pp);
			}
			else
			{//ZigZig
				leftRotate(pp);
				leftRotate(p);
			}
		}
	}
	root=T;
}
void Insert(int v)
{
	if(!root)
	{
		root=(Node *)malloc(sizeof(Node));
		root->l=NULL;
		root->r=NULL;
		root->p=NULL;
		root->v=v;
		return;
	}
	Node *P=root;
	while(1)
	{
		if(P->v==v) break; // not multiset
		if(v < (P->v) )
		{
			if(P->l) P=P->l;
			else
			{
				P->l=(Node *)malloc(sizeof(Node));
				P->l->p=P;
				P->l->r=NULL;
				P->l->l=NULL;
				P->l->v=v;
				P=P->l;
				break;
			}
		}
		else
		{
			if(P->r) P=P->r;
			else
			{
				P->r=(Node *)malloc(sizeof(Node));
				P->r->p=P;
				P->r->r=NULL;
				P->r->l=NULL;
				P->r->v=v;
				P=P->r;
				break;
			}
		}
	}
	Splay(P);
}
void Inorder(Node *R)
{
	if(!R) return;
	Inorder(R->l);
	printf("v: %d ",R->v);
	if(R->l) printf("l: %d ",R->l->v);
	if(R->r) printf("r: %d ",R->r->v);
	puts("");
	Inorder(R->r);
}
Node* Find(int v)
{
	if(!root) return NULL;
	Node *P=root;
	while(P)
	{
		if(P->v==v)
			break;
		if(v<(P->v))
		{
			if(P->l)
				P=P->l;
			else
				break;
		}
		else
		{
			if(P->r)
				P=P->r;
			else
				break;
		}
	}
	Splay(P);
	if(P->v==v) return P;
	else return NULL;
}
int Erase(int v)
{
	Node *N=Find(v);
	if(!N) return 0;
	Splay(N); //check once more;
	Node *P=N->l;
	if(!P)
	{
		root=N->r;
		root->p=NULL;
		free(N);
		return 1;
	}
	while(P->r) P=P->r;
	if(N->r)
	{
		P->r=N->r;
		N->r->p=P;
	}
	root=N->l;
	root->p=NULL;
	free(N);
	return 1;
}
int main()
{
	while(1)
	{
		int t;
		scanf("%d",&t);
		if(t!=0 && t!=-1) Insert(t);
		else if(t==0)
		{
			scanf("%d",&t);
			if(!Find(t))
				printf("Couldn't Find %d!\n",t);
			else
				printf("Found %d!\n",t);
		}
		else
		{
			scanf("%d",&t);
			if(Erase(t))
				printf("Deleted %d!\n",t);
			else
				printf("Couldn't Find %d!\n",t);
		}
		if(root) printf("root: %d\n",root->v);
		Inorder(root);
	}
}
