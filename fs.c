#include <stdio.h>
#include "fs.h"

void init_root()
{
	root = (Inode*)malloc(sizeof(Inode));
	root->path = "/";
	root->name = "/";
	root->type = 1;
	//Once persistence is acheived load files and update accordingly
	root->no_of_children = 0;
	root->no_of_files = 0;
	root->size = 0;
	root->inode_num = 0;
	root->parent = NULL;
	root->children = NULL;
	root->file.data = "";
	root->file.size = 0;
}

void init_node(const char * path, char * name, Inode *parent,int type)
{
	printf("Initializing Tree Node\n");
    Inode *new_node =(Inode*)malloc(sizeof(Inode));
    new_node->path = (char *)calloc(sizeof(char), strlen(path) + 1);
    new_node->name = (char *)calloc(sizeof(char), strlen(name) + 1);
    strcpy(new_node->path, (char *)path);
    strcpy(new_node->name, (char *)name);
    new_node->type =type;
    new_node->no_of_children = 0;
    new_node->size = 0;
    new_node->inode_num = parent->inode_num + parent->no_of_children;
    new_node->parent = parent;
    new_node->children = NULL;
    new_node->file.data = "";
    new_node->file.size = 0;
    return new_node;	
}

static int sys_getattr(const char* path, struct stat *st)
{
	printf("Get Attribute Called\n");
	//Find inode of that file
	Inode I = //found inode
	File F = found file;
	if(I.type == 1)	//directory
	{
		st->st_mode = S_IFDIR | 0777;
		st->st_nlink = 2;
	}
	else if(I.type == 0)	//file
	{
		st->st_mode = S_IFREG | 0777;
		st->st_nlink = 1;
		st->st_size = f->size;
		st->st_blocks = (((st->st_size) / 512) + 1);
	}
	else
		return -ENOENT;
	st->nlink = no_of_children;
	st->st_atime = time(NULL);
	st->st_mtime = time(NULL);
	return 0;
}

static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{
	printf("Read Directory Called\n");
	filler(buffer, ".", NULL, 0);
	filler(buffer, "..", NULL, 0);
	Inode I;
	if(strcmp(path, "/") == 0)
		I = root;
	else
		I = //found inode
	if(I == NULL)
		return -ENOENT;

	for(i=0; i<I->no_of_children; i++)
		filler(buffer, I->children[i]->name, NULL, 0);
	return 0;
}