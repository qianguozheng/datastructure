#include <stdlib.h>
#include <stdio.h>

int max(int a, int b){return a > b ? a:b;}

typedef struct node
{
	int val;
	struct node *left;
	struct node *right;
	struct node *parent;
	int height;
} node_t;

node_t *find(node_t *root, int val)
{
	if (NULL == root) return NULL;
	if (val < root->val)
		return find(root->left, val);
	else if (val > root->val)
		return find(root->right, val);
	else
		return root;
}

int height(node_t *root)
{
	return root ? root->height : 0;
}

void adjust_height(node_t *root)
{
	root->height = 1 + max(height(root->left), height(root->right));
}

/* we can assume node->left is non-null due to how this is called*/
node_t *rotate_right(node_t *root)
{
	node_t *new_root = root->left;
	if (root->parent)
	{
		if (root->parent->left == root) root->parent->left = new_root;
		else root->parent->right = new_root;
	}
	
	new_root->parent = root->parent;
	root->parent = new_root;
	root->left = new_root->right;
	if (root->left) root->left->parent = root;
	new_root->right = root;
	
	//Fix heights; root and new_root may be wrong. Do bottom-up
	adjust_height(root);
	adjust_height(new_root);
	return new_root;
}

node_t *rotate_left(node_t *root)
{
	node_t *new_root = root->right;
	if(root->parent)
	{
		if (root->parent->right == root) root->parent->right = new_root;
		else root->parent->left = new_root;
	}
	new_root->parent = root->parent;
	root->parent = new_root;
	root->right = new_root->left;
	if (root->right) root->right->parent = root;
	new_root->left = root;
	
	adjust_height(root);
	adjust_height(new_root);
	return new_root;
}

node_t *make_node(int val, node_t *parent)
{
	node_t *n = malloc(sizeof(node_t));
	n->val = val;
	n->parent = parent;
	n->height = 1;
	n->left = NULL;
	n->right = NULL;
	
	return n;
}

node_t *balance(node_t *root)
{
	if (height(root->left) - height(root->right) > 1)
	{
		if (height(root->left->left) > height(root->left->right))
		{
			root = rotate_right(root);
		}
		else
		{
			rotate_left(root->left);
			root = rotate_right(root);
		}
	}
	else if (height(root->right) - height(root->left) > 1)
	{
		if (height(root->right->right) > height(root->right->left))
		{
			root = rotate_left(root);
		}
		else
		{
			rotate_right(root->right);
			root = rotate_left(root);
		}
	}
	return root;
}

node_t *insert(node_t *root, int val)
{
	node_t *current = root;
	while(current->val != val)
	{
		if (val < current->val)
		{
			if (current->left) current = current->left;
			else
			{
				current->left=make_node(val, current);
				current = current->left;
			}
		}
		else if (val > current->val)
		{
			if(current->right) current = current->right;
			else
			{
				current->right = make_node(val, current);
				current = current->right;
			}
		}
		else return root;
	}
	
	do
	{
		current = current->parent;
		adjust_height(current);
		current = balance(current);
	}while(current->parent);
	
	return current;
}

void print_tree_indent(node_t *node, int indent)
{
	int ix;
	for (ix=0;ix<indent;ix++)printf(" ");
	if (!node) printf("Empty child\n");
	else
	{
		printf("node: %d; height:%d\n", node->val, node->height);
		print_tree_indent(node->left, indent+4);
		print_tree_indent(node->right, indent+4);
	}
}


void print_tree(node_t *node)
{
	print_tree_indent(node, 0);
}

int main(int argc, char *argv[])
{
	node_t *root = make_node(1, NULL);
	root = insert(root, 2);
	root = insert(root, 3);
	root = insert(root, 4);
	root = insert(root, 5);
	
	print_tree(root);
}
