#include <stdio.h>
#include "fs.h"
//HELPER FUNCTIONS
char *extractPath(char ** copy_path){
	printf("Path being extracted\n");
    char *path = (char *)calloc(sizeof(char), 1);
    int p_len = 0;
    char temp;
    char *tempstr;
    temp = **(copy_path);
    while(temp != '\0')
    {    
        if(temp == '/')
        {
            if(strlen(*copy_path) > 1)
            {
                (*copy_path)++;
            }
            break;
        }
        tempstr = (char *)calloc(sizeof(char) , (p_len + 2));
        strcpy(tempstr, path);
        p_len += 1;
        tempstr[p_len - 1] = temp;
        path = (char *)realloc(path, sizeof(char) * (p_len + 2));
        strcpy(path, tempstr);
        (*copy_path)++;
        temp = **(copy_path);
        free(tempstr);
    }
    path = (char *)realloc(path, sizeof(char) * (p_len + 1));
    path[p_len] = '\0';
    return path;
}

char *reverse(char *str, int mode){
	printf("Reversing String\n");
    int i;
    int len = strlen(str);
    char *tempstr = (char *)calloc(sizeof(char), (len + 1));
    for(i = 0; i <= len/2; i++){
        tempstr[i] = str[len - 1 -i];
        tempstr[len - i - 1] = str[i];
    }
    if(tempstr[0] == '/' && mode == 1)
    {
        /*if mode is set to 1 then it is a directory
        and return the string without the leading '/'
        */ 
        tempstr++;
    }
    return tempstr;
}


char *extractDir(char ** copy_path){
	printf("Extracting Directory Name\n");
    char *dir = (char *)calloc(sizeof(char), 1);
    int d_len = 0;
    char temp;
    char * tempstr;
    *copy_path = reverse(*copy_path, 1);    // change "a/b/c" to "c/b/a" and extract content upto the first '/'
    temp = **(copy_path);
    while(temp != '/'){    
        tempstr = (char *)calloc(sizeof(char), (d_len + 2));
        strcpy(tempstr, dir);
        d_len += 1;
        tempstr[d_len - 1] = temp;
        dir = (char *)realloc(dir, sizeof(char) * (d_len + 1));
        strcpy(dir, tempstr);
        (*copy_path)++;
        temp = **(copy_path);
        free(tempstr);
    }
    if(strlen(*copy_path) > 1){
        (*copy_path)++;                     // remove the leading '/' from "/b/a" after extracting 'c'
    }
    dir = (char *)realloc(dir, sizeof(char) * (d_len + 1));
    dir[d_len] = '\0';
    dir = reverse(dir, 0);        
    *(copy_path) = reverse(*(copy_path), 0);
    return dir;
}
//END OF HELPER FUNCTIONS

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
    new_node->child_inode = NULL;
    new_node->parent = parent;
    new_node->children = NULL;
    new_node->file.data = "";
    new_node->file.size = 0;
    insert_node(new_node);
    return new_node;    
}

//insert node function
void insert_node(Inode *node)
{
    Inode *parent;
    parent = node->parent;
    //parent = searchNode(node);

    if(parent->children!=NULL)
    {
        parent->children=(Inode**)realloc(parent->children,sizeof(Inode*) * parent->no_of_children+1);
        parent->child_inode =(int*)realloc(parent->child_inode, sizeof(int)*parent->no_of_children+1)
    }
    else
    {
    	parent->children=(Inode**)malloc(sizeof(Inode*));
    	parent->child_inode = (int*)malloc(sizeof(int));
    }
    parent->children[parent->no_of_children] = node;
    parent->child_inode[parent->no_of_children]=node->inode_num;
    parent->no_of_children++;
}
//end of insert node

Inode* search(Inode *node, char *name)
{
    int i;
    Inode* result;
    int no_of_children = node_name->no_of_children;
    if(!(strcmp(node->name,name)))
        return node;

    if(no_of_children == 0)
        return NULL;

    for(i = 0;i < no_of_children;i++)
    {
        if(node->children[i]->type == 1)
        {
            if(!(strcmp(node->children[i]->name,name)))
                return node;

            result = search(node->children[i],name);
            if(result != NULL)
                return result;
        }
        else
        {
            if(!(strcmp(node_name->children[i]->name,name)))
                return node_name;
        }
    }
    return NULL;
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
//START OF DELETE NODE
int deleteNode(const char *path)
{
    char *filename = find_file_name(path);//directory node which has the specified file name;
    Inode *node = NULL;
    node = search(root,filename);

    if(strcmp(filename,"/") && strcmp(node->parent->name,"/"))
    {
        return -1;
    }
    if(node->parent->no_children==0)
    {
        return -1;
    }

    if(node->parent->no_of_children==1 && (!strcmp(node->parent->children[0]->name,filename)))
    {
        return -1;
    }
    else
    {
        free(node->parent->children[0])
        //DISK CHANGES HAVE TO BE MADE HERE SINCE FILE IS GETTING DELETED
        node->parent->no_of_children--;
        return 0;

    }

    int i;
    int flag=0;
    int pos;
    for(i=0;i<node->parent->no_of_children;i++)
    {
        if(strcmp(node->parent->children[i]->name),filename)
        {
            flag=1;
            pos=i;
            break;
        }
    }

    if(flag==0)
    {
        return -1;
    }

    if(flag==1)
    {
        int j;
        for(j=i;i<node->parent->no_of_children-1;j++)
        {
            node->parent->children[j]=node->parent->children[j+1];
        }
	node->parent->no_of_children--;
        node->parent->child_inode = realloc(node->parent->child_inode,node->parent->no_of_children*(sizeof(int)));
        //DISK CHANGES HAVE TO BE MADE HERE SINCE FILE IS GETTING DELETED
        
        return 0;
    }
}


char *find_file_name(char *path)
{
    char *str = basename(char *path); //basename returns the name of the file;
    //it is present in <libgen.h>
}
//END OF DELETE NODE

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
