#include <stdio.h>
#include <stdlib.h>
#include <fuse.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#include "fs.h"
//HELPER FUNCTIONS
char *extractPath(char ** copy_path)
{
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

char *reverse(char *str, int mode)
{
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


char *extractDir(char *path){
	printf("EXTRACT DIR CALLED\n");
	/*printf("Extracting Directory Name\n");
    char *dir = (char *)calloc(sizeof(char), 1);
    int d_len = 0;
    char temp;
    char * tempstr;
    *copy_path = reverse(*copy_path, 1);    // change "a/b/c" to "c/b/a" and extract content upto the first '/'
    temp = **(copy_path);
    printf("TEMP: %s\n", *copy_path);
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
    return dir;*/
    /*if(strcmp(path,"/")==0)
		return path;
  	char str[strlen(path)];
  	strcpy(str,path);
    char* token = strtok(str, "/");
    printf("extractdir_token: %s\n",token );
    char*prev_token = (char*)malloc(sizeof(char)*strlen(path)); 
    strcpy(prev_token, "/");
    char* token_dir = (char*)malloc(sizeof(char)*strlen(path));
    while (token != NULL) { 
    	printf("in while of extract dir\n");
    	strcpy(token_dir, prev_token);
    	strcpy(prev_token,token);
        token = strtok(NULL, "/"); 
        printf("DIRECTORY: %s\n", token_dir);
        printf("PrevTOKEN: %s\n", prev_token);
        printf("TOKEN: %s\n", token);
    } 
    printf("out while of extract dir\n");
    printf("DIRECTORY: %s\n", token_dir);
    return token_dir;*/
    printf("exname_dir:%s\n", path);
	if(strcmp(path,"/")==0)
		return path;
  	char str[strlen(path)];
  	strcpy(str,path);
    char* token = strtok(str, "/"),*prev_token = malloc(sizeof(char)*strlen(path)), *token_dir = malloc(sizeof(char)*strlen(path)); 
    strcpy(prev_token, "/");
    while (token != NULL) { 
    	strcpy(token_dir, prev_token);
    	strcpy(prev_token,token);
        token = strtok(NULL, "/"); 
        printf("TOKENDIR: %s\n", token_dir);
        printf("PrevTOKEN: %s\n", prev_token);
        printf("TOKEN: %s\n", token);

    } 
    printf("DIR NAME: %s\n", token_dir);
    return token_dir;
}
//END OF HELPER FUNCTIONS

char *extract_name(char *path)
{
	printf("exname_PATH:%s\n", path);
	if(strcmp(path,"/")==0)
		return path;
  	char str[strlen(path)];
  	strcpy(str,path);
    char* token = strtok(str, "/"),*prev_token = malloc(sizeof(char)*strlen(path)); 
    while (token != NULL) { 
    	strcpy(prev_token,token);
        token = strtok(NULL, "/"); 
        printf("PrevTOKEN: %s\n", prev_token);
        printf("TOKEN: %s\n", token);

    } 
    printf("NAME: %s\n", prev_token);
    return prev_token;
} 

void init_root()
{
	printf("Initializing Root\n");
	root = (Inode*)malloc(sizeof(Inode));
	root->path = "/";
	root->name = "/";
	root->type = 1;
	//Once persistence is acheived load files and update accordingly
	root->no_of_children = 0;
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
    printf("Type: %d\n", new_node->type);
    new_node->no_of_children = 0;
    new_node->size = 0;
    new_node->inode_num = parent->inode_num + parent->no_of_children;
    new_node->child_inode = NULL;
    new_node->parent = parent;
    new_node->children = NULL;
    new_node->file.data = NULL;
    new_node->file.size = 0;
    insert_node(new_node);
  //  return new_node;    
}

//insert node function
void insert_node(Inode *node)
{
	printf("INsert node called\n");
    Inode *parent;
    parent = node->parent;
    //parent = searchNode(node);
    printf("parent: %s\n", parent->name);
    if(parent->children!=NULL)
    {
        parent->children=(Inode**)realloc(parent->children,sizeof(Inode*) * parent->no_of_children+1);
        parent->child_inode =(int*)realloc(parent->child_inode, sizeof(int)*parent->no_of_children+1);
    }
    else
    {
    	parent->children=(Inode**)malloc(sizeof(Inode*));
    	parent->child_inode = (int*)malloc(sizeof(int));
    }
    parent->children[parent->no_of_children] = node;
    parent->child_inode[parent->no_of_children]=node->inode_num;
    parent->no_of_children++;
    printf("Inserted node type: %d\n", parent->children[parent->no_of_children-1]->type);
    printf("Inserted node: %s\n", parent->children[parent->no_of_children-1]->name);
}
//end of insert node

Inode* search(Inode *node, char *name)
{
	printf("Search Node\n");
    int i;
    Inode* result;
    int no_of_children = node->no_of_children;
    printf("%s\n", name);
    if(!(strcmp(node->name,name)))
    {
    	printf("NODE RETURNED\n");
        return node;
    }

    if(no_of_children == 0)
        return NULL;

    for(i = 0;i < no_of_children;i++)
    {
        if(node->children[i]->type == 1)
        {
            if(!(strcmp(node->children[i]->name,name)))
            {
                	printf("CHILD RETURNED\n");
                    printf("CHILD: %s\n", node->children[i]->name);
                return node->children[i];
            }

            result = search(node->children[i],name);
            if(result != NULL)
                return result;
        }
        else
        {
        	printf("CHILD TYPE 0\n");
            if(!(strcmp(node->children[i]->name,name)))
            {
            	printf("CHILD NODE RETURNED\n");
            	printf("CHILD NAME%s\n", node->children[i]->name);
                return node->children[i];
            }
        }
    }
    return NULL;
}

char * read_data(Inode* node)
{
	printf("READ DATA\n");
	//Inode* inode = search(root, path);
	//int inode_number = temp->inode_num;
	printf("read_datafilesize: %d\n", node->file.size);
	char *temp = (char *)malloc(sizeof(char)*node->file.size+1);
	strcpy(temp,node->file.data);
		printf("read_datafilesize: %s\n", temp);
	//write_into_disk();
	return temp;
}

int write_data(Inode* inode,char *data)
{
	//int inode_number = find_path(path);
	//printf("%d\n",inode_number );
	//inode_blocks[inode_number].size = strlen(data);
	//printf("hi\n");
	printf("WRITE DATA\n");
	printf("%d\n", inode->file.size);
	if(inode->file.data == NULL)
	{
		inode->file.size = strlen(data);
		inode->file.data = (char*)malloc(inode->file.size * sizeof(char));
		strcpy(inode->file.data,data);
	}
	else
	{
		inode->file.size += strlen(data);
		inode->file.data = (char*)realloc(inode->file.data, inode->file.size * sizeof(char));
		strcat(inode->file.data, data);
	}
	//write_into_disk();
	return strlen(data);
}

static int sys_getattr(const char* path, struct stat *st)
{
	printf("Get Attribute Called\n");
	//Find inode of that file
	char* temp_path = path;
	printf("%s\n", path);
	char* temp_name = extract_name(temp_path);
	printf("getattr_tempname: %s\n", temp_name);

	Inode *I = search(root, temp_name);//found inode
	printf("Searched\n");
	if(I == NULL)
		return -ENOENT;
	printf("getattr_name: %s\n", I->name);

	File F = I->file;
	if(I->type == 1)	//directory
	{
		st->st_mode = S_IFDIR | 0777;
		st->st_nlink = 2;
	}
	else if(I->type == 0)	//file
	{
		printf("Regular file\n");
		st->st_mode = S_IFREG | 0777;
		st->st_nlink = 1;
		st->st_size = F.size;
		st->st_blocks = (((st->st_size) / 512) + 1);
	}
	else
	{
		printf("Get att returned enoent\n");
		return -ENOENT;
	}
	st->st_nlink = I->no_of_children;
	st->st_atime = time(NULL);
	st->st_mtime = time(NULL);
	printf("Get Attribute over\n");
	return 0;
}

static int sys_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{
	printf("Read Directory Called\n");
	filler(buffer, ".", NULL, 0);
	filler(buffer, "..", NULL, 0);
	Inode *I;
	int i;
	char* temp_path = path;
	if(strcmp(path, "/") == 0)
	{
        printf("IN IF\n");
		printf("PATH IS ROOT\n");
		I = root;
	}
	else
	{
        printf("IN ELSE\n");
		char* temp_name = extract_name(temp_path);
        printf("readdir_tempname: %s\n", temp_name);
		I = search(root, temp_name);
		printf("PATH IS %s", I->name);//found inode
		printf("CHildren: %d\n",I->no_of_children );
	}
	if(I == NULL)
		return -ENOENT;

	for(i=0; i<I->no_of_children; i++)
		filler(buffer, I->children[i]->name, NULL, 0);
	return 0;
}

static int sys_mkdir(const char * path, mode_t x){
	int type = 1;
	char* temp_path = path;
	char* temp_dir = extractDir(temp_path);
	char* name = extract_name(temp_path);
	Inode* parent = search(root, temp_dir);
	printf("mkdir_Parent: %s\n", parent->name);
	init_node(temp_path, name, parent, type);
	printf("Made directory\n");
//	write_into_disk();
	return 0;
}

static int sys_mknod(const char * path, mode_t x, dev_t y){
	printf("Make Node\n");
	int type = 0;
	char* temp_path = path;
	char* temp_dir = extractDir(temp_path);
	printf("mknode_tempdir: %s\n", temp_dir);
	char* name = extract_name(temp_path);
	Inode* parent = search(root, temp_dir);
	printf("mknodPARENT: %s\n", parent->name);
	init_node(temp_path, name, parent, type);
//	write_into_disk();
	return 0;
}

static int sys_open(const char *path, struct fuse_file_info *fi)	//return inode number
{
	char* temp_path = path;
	char* temp_name = extract_name(temp_path);
	Inode* file = search(root, temp_name);
    int file_pointer;
    if(file != NULL)
     	file_pointer = file->inode_num;
 	else
 		file_pointer = -1;
    if(file_pointer < 0) {  
        fprintf(stderr, "Open()\t Error: File [%s] does not exist.\n",temp_name);
        return -1;
    }
    printf("Open()\t : File [%s] opened.\n", temp_name);
    return 0;
}

static int sys_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
	printf("Read called\n");
	char* temp_path = path;
	char* temp_name = extract_name(temp_path);
	Inode* temp = search(root, temp_name);
	char * data=read_data(temp);
	printf("read_datafilesize: %s\n", data);
	memcpy(buffer,  data + offset, size);
	//write_into_disk();
	return size;
}

static int sys_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	printf("WRITE called and path is :%s\n",path);
	char* temp_path = path;
	char* temp_name = extract_name(temp_path);
	Inode* temp = search(root, temp_name);
	printf("write_temp: %s\n", temp->name);
	if(temp == NULL){
		return 0;
	}
	write_data(temp,buf);
//	write_into_disk();
	return size;	
}



int main( int argc, char *argv[] ){
	int i;
	/*for(i=0;i<MAX_BLOCKS;i++){
		i_bitmap[i]=0;
		d_bitmap[i]=0;
	}
	make_node("/","d");
	find_path("/");
	make_node("/abc.txt","f");
	write_data("/abc.txt","hello world");*/
	//global FILE *fp = fopen("/home/hduser/Desktop/2.txt","w+");
	//printf("%s\n",file_blocks[inode_blocks[1].blk_no[0]].data);
	//printf("%s\n",read_data("/abc.txt"));
	//return 0;	
//	read_from_disk();
	//printf("d_bitmap %d\n",i_bitmap[0]);
	init_root();
	return fuse_main( argc, argv, &operations);
}
