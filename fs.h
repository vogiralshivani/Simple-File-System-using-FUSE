#include <stdio.h>

typedef struct Inode
{
	char* path;
	char* name;
	int type;
	int no_of_children;
	int size;
	int inode_num;
	int child_inode[no_of_children];
	Inode *parent;
	Inode **children;
	File file;
} Inode;

typedef struct File
{
	char* data;
	int size;
}File;

#define INISIZE 1048576	//INIT_SIZE
#define BLKSIZE 4096	//BLOCK_SIZE

Inode *root;
