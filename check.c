#include <stdio.h>
typedef struct File
{
	char* data;
	int size;
}File;

typedef struct Inode
{
	char* path;
	char* name;
	int type;
	int no_of_children;
	int size;
	int inode_num;
	int* child_inode;
	struct Inode *parent;
	struct Inode **children;
	File file;
} Inode;
void main()
{
	FILE *fp;
	fp = fopen("fsdata","rb");
	printf("here\n");
	Inode *node;
	node = (Inode*)malloc(sizeof(Inode));
	while(!feof(fp))
	{
		//printf("in while\n");
		fread(node,sizeof(Inode),1,fp);
		printf ("inode_num = %d\n", (node->inode_num)); 

	}
}
