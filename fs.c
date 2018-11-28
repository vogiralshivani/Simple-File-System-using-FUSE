#include <stdio.h>
#include <stdlib.h>
#include <fuse.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#include "fs.h"
int GLOBAL_INUM = 0;

int run_flag = 0;


char* metafilename = "fsmeta";
char *datafilename = "fsdata";

meta_node* init_meta_node(Inode* I)
{
    int i;
    printf("init_meta_node function called\n");
    meta_node *temp = (meta_node*)malloc(sizeof(meta_node));
    temp->pathlength = strlen(I->path);
    temp->path = (char*)malloc(strlen(I->path)*sizeof(char));
    strcpy(temp->path, I->path);
    temp->namelength = strlen(I->name);
    temp->name = (char*)malloc(strlen(I->name)*sizeof(char));
    strcpy(temp->name, I->name);
    temp->type = I->type;
    temp->size = I->size;
    temp->inode_num =I->inode_num;
    if(I->inode_num != 0)
    {
        temp->parent_namelength = strlen(I->parent->name);
        temp->parent_name = (char*)malloc(strlen(I->parent->name)*sizeof(char));
        strcpy(temp->parent_name, I->parent->name);
    }
    else
    {
        temp->parent_namelength = 0;
        temp->parent_name = NULL;
    }
    temp->file.size = I->file.size;
    if(I->file.data != NULL)
    {
        temp->file.data = (char*)malloc(strlen(I->file.data)*sizeof(char));
        strcpy(temp->file.data, I->file.data);
    }
    return temp;
}

void read_from_datafile(meta_node *disk)
{
    printf("read_from_datafile function called\n");
    FILE* dfp = fopen("fsdata", "rb");
    int tmp;

    while(feof(dfp)==0)
    {
        tmp = fgetc(dfp);
        if(tmp!=EOF)
        {   
            ungetc(tmp, dfp);
            data_node *regfile = (data_node*)malloc(sizeof(data_node));
            fread(&regfile->inode_num, sizeof(regfile->inode_num), 1, dfp);
            fread(&regfile->size, sizeof(regfile->size), 1, dfp);

            regfile->data = (char*)malloc(regfile->size*sizeof(char));

            fread(regfile->data, regfile->size*sizeof(char), 1, dfp);

            if(regfile->inode_num == disk->inode_num)
            {
                disk->file.data = (char*)malloc(disk->file.size*sizeof(char));
                strcpy(disk->file.data, regfile->data);
                free(regfile);

               
                break;
            }
            
            free(regfile);
        }
    } 
    fclose(dfp);
}

void read_from_disk_wrapper()
{
    printf("read_from_disk_wrapper function called\n");
    int i;
    FILE *mfp = fopen(metafilename,"rb");

    meta_node *temp_disk = (meta_node*)malloc(sizeof(meta_node));
    read_from_disk(temp_disk,mfp);

    free(temp_disk);
    int tmp;
    while(feof(mfp)==0)
    {

        tmp = fgetc(mfp);
        if(tmp!=EOF)
        {
            ungetc(tmp, mfp);
            meta_node *temp_disk = (meta_node*)malloc(sizeof(meta_node));
            read_from_disk(temp_disk, mfp);
            free(temp_disk);
        }
    } 

    fclose(mfp);
}

void read_from_disk(meta_node* disk, FILE *mfp)
{
    printf("read_from_disk function called\n");
    int i;
    fread(&disk->pathlength, sizeof(disk->pathlength), 1, mfp);
    disk->path = (char*)malloc(disk->pathlength*sizeof(char)+1);
    fread(disk->path, sizeof(disk->path), 1, mfp);
    disk->path[disk->pathlength] = '\0';
    fread(&disk->namelength, sizeof(disk->namelength), 1, mfp);
    disk->name = (char*)malloc(disk->namelength*sizeof(char)+1);
    fread(disk->name, sizeof(disk->name), 1, mfp);
    disk->name[disk->namelength] = '\0';
    
    fread(&disk->parent_namelength, sizeof(disk->parent_namelength), 1, mfp);
    if(disk->parent_namelength != 0)
    {
        disk->parent_name = (char*)malloc(disk->parent_namelength*sizeof(char)+1);
        fread(disk->parent_name, sizeof(disk->parent_name), 1, mfp);
        disk->parent_name[disk->parent_namelength] = '\0';

    }
    else
        disk->parent_name = NULL;

    fread(&disk->type, sizeof(disk->type), 1, mfp);

    
    fread(&disk->size, sizeof(disk->size), 1, mfp);


    fread(&disk->inode_num, sizeof(disk->inode_num), 1, mfp);

    fread(&disk->file.size, sizeof(disk->file.size), 1, mfp);

    if(disk->file.size == 0)
    {
        printf("FILE ZERO\n");
    }
    else
    {
        read_from_datafile(disk);
        
    }

 
    Inode *new_node = (Inode*)malloc(sizeof(Inode));
    new_node->path = (char*)malloc(disk->pathlength*sizeof(char));
    strcpy(new_node->path, disk->path);
    new_node->name = (char*)malloc(disk->namelength*sizeof(char));
    strcpy(new_node->name, disk->name);
    if(disk->parent_name!= NULL)
    {
        Inode *pnode = search(root,disk->parent_name);
        new_node->parent = pnode;
    }
    else
        new_node->parent = NULL;


    new_node->type = disk->type;
    new_node->no_of_children = 0;
    new_node->size = disk->size;
    new_node->inode_num = disk->inode_num;
    new_node->file.size = disk->file.size;
    if(new_node->file.size != 0)
    {
        new_node->file.data = (char*)malloc(sizeof(char)*new_node->file.size);
        strcpy(new_node->file.data, disk->file.data);
    }
    else
        new_node->file.data = NULL;
    if(new_node->parent != NULL)
        insert_node(new_node);
    //return new_node;
}


void write_to_disk_wrapper()
{
     
    printf("write_to_disk_wrapper function called\n");
    FILE *mfp = fopen(metafilename,"wb");
    FILE *dfp = fopen(datafilename, "wb");
    //int i=0;
    Inode *temp = root;
    meta_node *temp_disk = init_meta_node(root);
    //writing root node
    //inode_bitmap[i] = 1;
    //data_bitmap[i] = 1;
    //inodeblocks[i] = *temp;
    //i++;
    //fwrite(temp_disk,sizeof(temp_disk),1,mfp);
    write_to_disk(temp_disk, mfp, dfp);
   
    free(temp_disk);
    int j;
    int n=temp->no_of_children;
    for(j=0;j<n;j++)
    {
       
        write_to_disk_recurse(temp->children[j],mfp, dfp);
    }

    fclose(mfp);
    fclose(dfp);

}

void write_to_disk_recurse(Inode *node, FILE *mfp, FILE *dfp)
{
  
    printf("write_to_disk_recurse function called\n");
    meta_node *temp_disk = init_meta_node(node);
 
    write_to_disk(temp_disk, mfp, dfp);
    
 
    free(temp_disk);

        int j=0;
        for(j=0;j<node->no_of_children;j++)
        {
            write_to_disk_recurse(node->children[j], mfp, dfp);
        }

    return;
}

void write_to_disk(meta_node* disk, FILE *mfp, FILE *dfp)
{
    printf("write_to_disk function called\n");
    fwrite(&disk->pathlength, sizeof(disk->pathlength), 1, mfp);
 
    fwrite(disk->path, sizeof(disk->path), 1, mfp);

    fwrite(&disk->namelength, sizeof(disk->namelength), 1, mfp);

    fwrite(disk->name, sizeof(disk->name), 1, mfp);

    fwrite(&disk->parent_namelength, sizeof(disk->parent_namelength), 1,mfp);

    if(disk->parent_name != NULL)
        fwrite(disk->parent_name, sizeof(disk->parent_name),1,mfp);

    fwrite(&disk->type, sizeof(disk->type), 1, mfp);

    fwrite(&disk->size, sizeof(disk->size), 1, mfp);

    fwrite(&disk->inode_num, sizeof(disk->inode_num), 1, mfp);

    fwrite(&disk->file.size, sizeof(disk->file.size), 1, mfp);

    
    if(disk->file.size != 0)
    {
       

        data_node *regfile = (data_node*)malloc(sizeof(data_node));
        regfile->inode_num = disk->inode_num;
        fwrite(&regfile->inode_num, sizeof(regfile->inode_num), 1, dfp);
        regfile->size = disk->file.size;
        fwrite(&regfile->size, sizeof(regfile->size), 1, dfp);
        regfile->data  = (char*)malloc(regfile->size*sizeof(char));
        strcpy(regfile->data, disk->file.data);
        fwrite(regfile->data, regfile->size * sizeof(char), 1, dfp);
        free(regfile);
    }
   
}
//HELPER FUNCTIONS

char *extractDir(const char *path){
    printf("extractDir function called\n");
    
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


    }
    return token_dir;
}
//END OF HELPER FUNCTIONS

char *extract_name(const char *path)
{
   printf("extract_name function called\n");
    if(strcmp(path,"/")==0)
        return path;
    char str[strlen(path)];
    strcpy(str,path);
    char* token = strtok(str, "/"),*prev_token = malloc(sizeof(char)*strlen(path)); 
    while (token != NULL) { 
        strcpy(prev_token,token);
        token = strtok(NULL, "/"); 
       

    } 
   
    return prev_token;
} 

void init_root()
{
    printf("init_root function called\n");
    root = (Inode*)malloc(sizeof(Inode));
    root->path = "/";
    root->name = "/";
    root->type = 1;
    root->no_of_children = 0;
    root->size = 0;
    root->inode_num = GLOBAL_INUM;
    root->parent = NULL;
    root->children = NULL;
    root->file.data = "";
    root->file.size = 0;
    GLOBAL_INUM ++;
}

void init_node(const char * path, char * name, Inode *parent,int type)
{
    printf("init_node function called\n");


Inode *new_node =(Inode*)malloc(sizeof(Inode));
    new_node->path = (char *)calloc(sizeof(char), strlen(path) + 1);
    new_node->name = (char *)calloc(sizeof(char), strlen(name) + 1);
    strcpy(new_node->path, (char *)path);
    strcpy(new_node->name, (char *)name);
    new_node->type =type;
    new_node->no_of_children = 0;
    new_node->size = 0;
    new_node->inode_num = GLOBAL_INUM ++;;
    new_node->child_inode = NULL;
    new_node->parent = parent;
    new_node->children = NULL;
    new_node->file.data = NULL;
    new_node->file.size = 0;
    GLOBAL_INUM ++;
  
    insert_node(new_node);
  //  return new_node;    
}

//insert node function
void insert_node(Inode *node)
{
    printf("insert_node function called\n");
    Inode *parent;
    parent = node->parent;
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
    
}
//end of insert node

//flag=0 means search based on name
//flag=1 means search based on inode numder
Inode* search(Inode *node, char *name)
{
    printf("search function called\n");
    int i;
    Inode* result;
    int no_of_children = node->no_of_children;
    if(!(strcmp(node->name,name)))
    {
        return node;
    }

    if(no_of_children == 0)
    {
        return NULL;
    }

    for(i = 0;i < no_of_children;i++)
    {
        if(node->children[i]->type == 1)
        {
            if(!(strcmp(node->children[i]->name,name)))
            {
               return node->children[i];
            }

            result = search(node->children[i], name);
            if(result != NULL)
                return result;
        }
        else
        {
       
            if(!(strcmp(node->children[i]->name,name)))
            {
                return node->children[i];
            }
        }
    }
        

    return NULL;
}

char * read_data(Inode* node)
{
    printf("read_data function called\n");
    char *temp = (char *)malloc(sizeof(char)*node->file.size+1);
    strcpy(temp,node->file.data);


    return temp;
}

int write_data(Inode* inode,const char *data)
{
    printf("write_data function called\n");
    if(inode->file.data == NULL)
    {
        inode->file.size = strlen(data);
       
        inode->file.data = (char*)malloc(inode->file.size * sizeof(char));
        strcpy(inode->file.data,data);
    }
    else
    {
        int i;
        int len = strlen(data);
        for(i=0; i< len; i++)
        {
            if(data[i] == '\n')
                break;
        }
        char* text  = (char*)malloc(i*sizeof(char));
        strncpy(text, data, i);
        inode->file.size += strlen(text);
        inode->file.data = (char*)realloc(inode->file.data, inode->file.size * sizeof(char));
        strcat(inode->file.data, text);
        free(text);
    }
    return strlen(data);
}

//delete node
int deleteNode(const char *path)
{
    printf("deleteNode function called\n");
    char *filename = extract_name(path);

    if(strcmp(filename,"/")==0)
    {
        printf("ERROR CANNOT DELETE ROOT DIRECTORY\n");
        return -1;
    }
    Inode *node = search(root,filename);

    if(node==NULL)
    {
        printf("ERROR FILE NOT FOUND\n");
        return -1;
    }

    if(node->type==1 && node->no_of_children>0)
    {
        printf("ERROR CANNOT DELETE DIRECTORY WITH FILES IN IT\n");
        return -1;
    }

    int temp_inum = node->inode_num;
    char *dirname = extractDir(path);
    Inode *parent = search(root,dirname);
   
    int i, j;
    for(i=0; i<parent->no_of_children; i++)
    {
       
        if(parent->children[i]->inode_num == node->inode_num)
        {
            for(j=i; j<parent->no_of_children-1; j++)
            {
                parent->children[j] = parent->children[j+1];
                parent->child_inode[j] = parent->child_inode[j+1];
            }
        }
    }
    parent->no_of_children--;
    if(parent->no_of_children == 0)
    {
        parent->children = NULL;
    }

    free(node);
    //GLOBAL_INUM--;
    return 0; //success

}

static int sys_getattr(const char* path, struct stat *st)
{
    printf("Get Attribute Called\n");
    //Find inode of that file
    const char* temp_path = path;
 
    char* temp_name = extract_name(temp_path);
   
    Inode *I = search(root, temp_name);//found inode

    if(I == NULL)
    {
        return -ENOENT;
    }

    File F = I->file;
    if(I->type == 1)    //directory
    {
        st->st_mode = S_IFDIR | 0777;
        st->st_nlink = 2;
    }
    else if(I->type == 0)   //file
    {

        st->st_mode = S_IFREG | 0777;
        st->st_nlink = 1;
        st->st_size = F.size;
        st->st_blocks = (((st->st_size) / 512) + 1);
    }
    else
    {
        return -ENOENT;
    }
    st->st_nlink = I->no_of_children;
    st->st_atime = time(NULL);
    st->st_mtime = time(NULL);
    return 0;
}

static int sys_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{
    
    printf("Readdir called\n");
    filler(buffer, ".", NULL, 0);
    filler(buffer, "..", NULL, 0);
    Inode *I;
    int i;
    const char* temp_path = path;
    if(strcmp(path, "/") == 0)
    {
        printf("Reading Root\n");
        I = root;
    }
    else
    {
        
        char* temp_name = extract_name(temp_path);
       
        I = search(root, temp_name);
 
    }
    if(I == NULL)
        return -ENOENT;

    for(i=0; i<I->no_of_children; i++)
        filler(buffer, I->children[i]->name, NULL, 0);
    return 0;
}

static int sys_mkdir(const char * path, mode_t x){
    printf("Mkdir called\n");
    int type = 1;
    const char* temp_path = path;
    char* temp_dir = extractDir(temp_path);
    char* name = extract_name(temp_path);
    Inode* parent = search(root, temp_dir);
 
    init_node(temp_path, name, parent, type);

    write_to_disk_wrapper();
    return 0;
}

static int sys_mknod(const char * path, mode_t x, dev_t y){
    printf("Make Node Called\n");
    int type = 0;
    const char* temp_path = path;
    char* temp_dir = extractDir(temp_path);

    char* name = extract_name(temp_path);
    Inode* parent = search(root, temp_dir);

    init_node(temp_path, name, parent, type);
    write_to_disk_wrapper();
    return 0;
}

static int sys_open(const char *path, struct fuse_file_info *fi)    //return inode number
{
    printf("Open Called\n");
    const char* temp_path = path;
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
    const char* temp_path = path;
    char* temp_name = extract_name(temp_path);
    Inode* temp = search(root, temp_name);
    char * data=read_data(temp);
    memcpy(buffer,  data + offset, size);
    return size;
}

static int sys_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf("Write called to path is :%s\n",path);
    const char* temp_path = path;
    char* temp_name = extract_name(temp_path);
    Inode* temp = search(root, temp_name);
    if(temp == NULL){
        return 0;
    }
    write_data(temp,buf);
    write_to_disk_wrapper();
    return size;    
}

static int sys_rmdir(const char *path)
{
    printf("Rmdir called for %s\n",path);
    int ret = deleteNode(path);
    if(ret < 0)
        return -1;
    write_to_disk_wrapper();
    return 0;
}

static int sys_unlink(const char *path)
{
    printf("Unlink called for %s\n",path);
    int ret = deleteNode(path);
    if(ret < 0)
        return -1;
    write_to_disk_wrapper();
    return 0;
}


void print_tree(Inode* node)
{
    Inode* temp=node;
    int i;
    printf("%s\n", node->name);
    for(i=0; i<node->no_of_children; i++)
    {
        printf("%s\n", node->children[i]->name);
        print_tree(node->children[i]);
    }
    return;

}

static int sys_rename(const char* old_name_path , const char* new_name_path)
{
    printf("Rename Called\n");
    char* old_name = extract_name(old_name_path);
    char* new_name = extract_name(new_name_path);
    Inode* node = search(root, old_name);
    node->name = new_name;
    write_to_disk_wrapper();
    return 0;
}


int main( int argc, char *argv[] )
{
    
   // Inode *root;
    init_root();
    FILE *filep = fopen("fsmeta", "rb");
    if(filep != NULL)
    {
        fclose(filep);
        read_from_disk_wrapper();
    }
    return fuse_main( argc, argv, &operations);
}
