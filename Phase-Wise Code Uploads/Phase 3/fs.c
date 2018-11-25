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

//int inode_bitmap[no_of_blocks];
//int data_bitmap[no_of_blocks];

//Inode inodeblocks[no_of_blocks];

char* metafilename = "fsmeta";
char *datafilename = "fsdata";

meta_node* init_meta_node(Inode* I)
{
    int i;
    printf("INIT META NODE\n");
    meta_node *temp = (meta_node*)malloc(sizeof(meta_node));
    temp->pathlength = strlen(I->path);
    temp->path = (char*)malloc(strlen(I->path)*sizeof(char));
    strcpy(temp->path, I->path);
    temp->namelength = strlen(I->name);
    temp->name = (char*)malloc(strlen(I->name)*sizeof(char));
    strcpy(temp->name, I->name);
    temp->type = I->type;
    //temp->no_of_children = I->no_of_children;
    temp->size = I->size;
    temp->inode_num =I->inode_num;
    if(I->inode_num != 0)
	{
        printf("INIT META NODE PARENT NAME %s\n", I->parent->name);
        temp->parent_namelength = strlen(I->parent->name);
        temp->parent_name = (char*)malloc(strlen(I->parent->name)*sizeof(char));
        strcpy(temp->parent_name, I->parent->name);
	}
    else
	{
        printf("INI META NODE IS ROOT\n");
        temp->parent_namelength = 0;
		temp->parent_name = NULL;
	}
    /*if(I->no_of_children > 0)
    {
        temp->child_inode = (int*)malloc(temp->no_of_children*sizeof(int));
        for(i=0; i<temp->no_of_children; i++)
            temp->child_inode[i] = I->child_inode[i];
    }
    else 
        temp->child_inode = NULL;*/

    temp->file.size = I->file.size;
    printf("I FILE SIZE: %d\n", I->file.size);
    if(I->file.data != NULL)
    {
        temp->file.data = (char*)malloc(strlen(I->file.data)*sizeof(char));
        strcpy(temp->file.data, I->file.data);
    }
    return temp;
}

void read_from_datafile(meta_node *disk)
{
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
	    	fread(regfile->data, sizeof(regfile->data), 1, dfp);
	    	if(regfile->inode_num == disk->inode_num)
	    	{
	    		disk->file.data = (char*)malloc(disk->file.size*sizeof(char));
	    		strcpy(disk->file.data, regfile->data);
	    		free(regfile);
	    		printf("FOUND\n");
	    		printf("%s\n", disk->file.data);
	    		break;
	    	}
	    	printf("in while : %d %d\n", regfile->inode_num, disk->inode_num);
	    	
			free(regfile);
		}
	} 
	fclose(dfp);
}

void read_from_disk_wrapper()
{
	int i;
	FILE *mfp = fopen(metafilename,"rb");
   // FILE *dfp = fopen(datafilename, "rb");
	meta_node *temp_disk = (meta_node*)malloc(sizeof(meta_node));
	read_from_disk(temp_disk,mfp);
//    init_root();
    free(temp_disk);
	int tmp;
    while(feof(mfp)==0)
    {
        printf("IN READ WHILE\n");
        tmp = fgetc(mfp);
        if(tmp!=EOF)
        {
            ungetc(tmp, mfp);
            meta_node *temp_disk = (meta_node*)malloc(sizeof(meta_node));
            read_from_disk(temp_disk, mfp);
            free(temp_disk);
        }
    } 
	/*for(i=0;i<n;i++)
	{
		//entry_node->children[i]->parent = entry_node;
        Inode* node;
		read_from_disk_recurse(node,mfp);
        printf("Node to be added: %s\n", node->name);
    //    root->children[i] = node;
	}*/
	fclose(mfp);
//	fclose(dfp);

}

/*void read_from_disk_recurse(Inode *node,FILE *mfp)
{
    //meta_node *temp_disk = init_meta_node(node);
    meta_node *temp_disk = (meta_node*)malloc(sizeof(meta_node));
	node = read_from_disk(temp_disk,mfp);
	int j;
	for(j=0;j<node->no_of_children;j++)
	{
	//	entry_node->children[j]->parent = entry_node;
        Inode* child;
		read_from_disk_recurse(child,mfp);
    //    node->children[j] = child;
	}
    //insert_node(new_node);

}*/

void read_from_disk(meta_node* disk, FILE *mfp)
{
	int i;
    fread(&disk->pathlength, sizeof(disk->pathlength), 1, mfp);
//    printf("%ld\n", sizeof(disk->pathlength));
//    printf("Pathlen read %d\n", disk->pathlength);
//   	printf("FILE POINTER: %ld\n", ftell(mfp));

    disk->path = (char*)malloc(disk->pathlength*sizeof(char)+1);
    fread(disk->path, sizeof(disk->path), 1, mfp);
//    printf("%ld\n", sizeof(disk->path));
//    printf("Path read %s\n", disk->path);
    disk->path[disk->pathlength] = '\0';
//    printf("FILE POINTER: %ld\n", ftell(mfp));

    fread(&disk->namelength, sizeof(disk->namelength), 1, mfp);
//    printf("%ld\n", sizeof(disk->namelength));
//    printf("Namelen read %d\n", disk->namelength);
//    printf("FILE POINTER: %ld\n", ftell(mfp));

    disk->name = (char*)malloc(disk->namelength*sizeof(char)+1);
    fread(disk->name, sizeof(disk->name), 1, mfp);
//    printf("%ld\n", sizeof(disk->name));
//    printf("Name read %s\n", disk->name);
    disk->name[disk->namelength] = '\0';
//    printf("FILE POINTER: %ld\n", ftell(mfp));
    
    fread(&disk->parent_namelength, sizeof(disk->parent_namelength), 1, mfp);
//    printf("%ld\n", sizeof(disk->parent_namelength));
//    printf("Namelen read %d\n", disk->parent_namelength);
//    printf("FILE POINTER: %ld\n", ftell(mfp));
    if(disk->parent_namelength != 0)
    {
        disk->parent_name = (char*)malloc(disk->parent_namelength*sizeof(char)+1);
        fread(disk->parent_name, sizeof(disk->parent_name), 1, mfp);
        disk->parent_name[disk->parent_namelength] = '\0';

    }
    else
        disk->parent_name = NULL;
//    printf("%ld\n", sizeof(disk->parent_name));
//    printf("Name read %s\n", disk->parent_name);
//    printf("FILE POINTER: %ld\n", ftell(mfp));

    fread(&disk->type, sizeof(disk->type), 1, mfp);
//    printf("%ld\n", sizeof(disk->type));
//	printf("Type read %d\n", disk->type);
//	printf("FILE POINTER: %ld\n", ftell(mfp));
    
//    fread(&disk->no_of_children, sizeof(disk->no_of_children), 1, mfp);
//    printf("%ld\n", sizeof(disk->no_of_children));
//    printf("No_children read %d\n", disk->no_of_children);
//	printf("FILE POINTER: %ld\n", ftell(mfp));
    
    fread(&disk->size, sizeof(disk->size), 1, mfp);
//    printf("%ld\n", sizeof(disk->size));
//    printf("Size read %d\n", disk->size);
//	printf("FILE POINTER: %ld\n", ftell(mfp));

    fread(&disk->inode_num, sizeof(disk->inode_num), 1, mfp);
//   printf("%ld\n", sizeof(disk->inode_num));    
//    printf("Inode_num read %d\n", disk->inode_num);
//	printf("FILE POINTER: %ld\n", ftell(mfp));
    
/*    if(disk->no_of_children == 0)
    	disk->child_inode = NULL;
    else
    {
    	disk->child_inode = (int*)malloc(disk->no_of_children*sizeof(int));
//    	printf("Size of child inode%ld\n", sizeof(disk->child_inode));
        fread(disk->child_inode, sizeof(disk->child_inode), 1, mfp);
//        for(i=0; i<2; i++)
//        	printf("Child_inode read %d\n", disk->child_inode[i]);
   	}*/
//   	printf("FILE POINTER: %ld\n", ftell(mfp));
    
    //fread(&disk->parent_inode, sizeof(disk->parent_inode), 1, mfp);
    //printf("%ld\n", sizeof(disk->parent_inode));    
    //printf("Parent read %d\n", disk->parent_inode);
	//printf("FILE POINTER: %ld\n", ftell(mfp));
    
    fread(&disk->file.size, sizeof(disk->file.size), 1, mfp);
//    printf("File size read %d\n", disk->file.size);
    if(disk->file.size == 0)
    {
    	printf("FILE ZERO\n");
    }
    else
    {
    	read_from_datafile(disk);
    	printf("data: %s\n", disk->file.data);
    }
    //else
    //{
    //	disk->file.data = (char*)malloc(disk->file.size* sizeof(char));
    //    fread(disk->file.data, sizeof(disk->file.size), 1, mfp);
    //}
//    printf("SIZE OF DISK %ld\n", sizeof(disk));
    printf("%ld\n", ftell(mfp));
//    printf("Filedata read\n");
	//meta_node has been modified now, we want the Inode to build the tree. 
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

//	new_node->parent_name = (char*)malloc(&disk->parent_namelength*sizeof(char));
//    strcpy(new_nodisk->parent_name;
	new_node->type = disk->type;
	new_node->no_of_children = 0;
	new_node->size = disk->size;
	new_node->inode_num = disk->inode_num;

/*    if(disk->no_of_children > 0)
    {
        new_node->child_inode = (int*)malloc(disk->no_of_children*sizeof(int));
        for(i=0; i<disk->no_of_children; i++)
            new_node->child_inode[i] = disk->child_inode[i];
        new_node->children = (Inode**)malloc(disk->no_of_children*sizeof(Inode*));
    }
    else 
    {
        new_node->child_inode = NULL;
        new_node->children = NULL;
    }*/
//            printf("INODE CREATED TILL CHILDREN\n");

//	new_node->child_inode = disk->child_inode;

//	for(i =0;i<new_node->no_of_children;i++)
//	{
//		new_node->children[i]=search(root,NULL,new_node->child_inode[i],1);
//	}
    new_node->file.size = disk->file.size;
        printf("new_node name %d\n", new_node->file.size);
    if(new_node->file.size != 0)
    {
        new_node->file.data = (char*)malloc(sizeof(char)*new_node->file.size);
        strcpy(new_node->file.data, disk->file.data);
    }
	else
        new_node->file.data = NULL;
    printf("NEW INODE CREATED\n");
    if(new_node->parent != NULL)
        insert_node(new_node);
	//return new_node;
}


void write_to_disk_wrapper()
{
    printf("here in disk write_to_disk_wrapper\n"); 
     
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
    printf("WRITTEN to DISK\n");
    free(temp_disk);
    int j;
    int n=temp->no_of_children;
    for(j=0;j<n;j++)
    {
        printf("in for\n");
        write_to_disk_recurse(temp->children[j],mfp, dfp);
    }
    printf("Closing\n");
    fclose(mfp);
    fclose(dfp);

}

void write_to_disk_recurse(Inode *node, FILE *mfp, FILE *dfp)
{
    printf("in write_to_disk recurse\n");
    //static int i=1;
    //if(node->type==0)
    //{
        //inode_bitmap[*i] = 1;
       
        //inodeblocks[*i] = *node;

        //*(i++);   
    meta_node *temp_disk = init_meta_node(node);
    printf("temp disk set\n");
    write_to_disk(temp_disk, mfp, dfp);
    //fwrite(temp_disk,sizeof(temp_disk),1,mfp);
    printf("WRITTEN TO DISK\n");
    free(temp_disk);
        //printf("i = %d\n",*i );

    //}   
    //else
    //{
        int j=0;
        for(j=0;j<node->no_of_children;j++)
        {
            write_to_disk_recurse(node->children[j], mfp, dfp);
        }
    //}
    return;
}

void write_to_disk(meta_node* disk, FILE *mfp, FILE *dfp)
{
    fwrite(&disk->pathlength, sizeof(disk->pathlength), 1, mfp);
    printf("Pathlen written\n");
    fwrite(disk->path, sizeof(disk->path), 1, mfp);
    printf("Path written. sizeof path: %ld\n", sizeof(disk->path));
    fwrite(&disk->namelength, sizeof(disk->namelength), 1, mfp);
    printf("Namelen written %d\n", disk->namelength);
    fwrite(disk->name, sizeof(disk->name), 1, mfp);
    printf("Name written\n %s", disk->name);
	fwrite(&disk->parent_namelength, sizeof(disk->parent_namelength), 1,mfp);
	printf("Parent Namelength written\n");
	if(disk->parent_name != NULL)
        fwrite(disk->parent_name, sizeof(disk->parent_name),1,mfp);
	printf("Parent Name Written. Length is %ld\n",sizeof(disk->parent_name));
    fwrite(&disk->type, sizeof(disk->type), 1, mfp);
    printf("Type written\n");
//    fwrite(&disk->no_of_children, sizeof(disk->no_of_children), 1, mfp);
//    printf("No_children written\n");
    fwrite(&disk->size, sizeof(disk->size), 1, mfp);
    printf("Size written\n");
    fwrite(&disk->inode_num, sizeof(disk->inode_num), 1, mfp);
    printf("Inode_num written\n");
//    if(disk->child_inode != NULL)
//        fwrite(disk->child_inode, sizeof(disk->child_inode), 1, mfp);
//    printf("Child_inode written\n");
//    fwrite(&disk->parent_inode, sizeof(disk->parent_inode), 1, mfp);
    printf("Parent written\n");
    fwrite(&disk->file.size, sizeof(disk->file.size), 1, mfp);
    printf("File size written %d\n", disk->file.size);
    
    if(disk->file.size != 0)
    {
        data_node *regfile = (data_node*)malloc(sizeof(data_node));
        regfile->inode_num = disk->inode_num;
        printf("WRITING TO DATA FILE %d\n", regfile->inode_num);
        fwrite(&regfile->inode_num, sizeof(regfile->inode_num), 1, dfp);
        regfile->size = disk->file.size;
        fwrite(&regfile->size, sizeof(regfile->size), 1, dfp);
        regfile->data  = (char*)malloc(regfile->size*sizeof(char));
        strcpy(regfile->data, disk->file.data);
        fwrite(regfile->data, sizeof(regfile->inode_num), 1, dfp);
        free(regfile);
    }
    printf("Filedata written\n");
}
//HELPER FUNCTIONS

char *extractDir(char *path){
    printf("EXTRACT DIR CALLED\n");
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
    //char* str = dirname(path);
    //printf("exDIR: %s\n", str);
    //return str;
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
    //char *str = basename(path);
    //printf("exNAME: %s\n", str);
    //return str;
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
    root->inode_num = GLOBAL_INUM;
    root->parent = NULL;
    root->children = NULL;
    root->file.data = "";
    root->file.size = 0;
    GLOBAL_INUM ++;
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
    new_node->inode_num = GLOBAL_INUM ++;;
    new_node->child_inode = NULL;
    new_node->parent = parent;
    new_node->children = NULL;
    new_node->file.data = NULL;
    new_node->file.size = 0;
    GLOBAL_INUM ++;
    printf("INUM INCREMENTED\n");
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
    printf("NO OF CHILDREN OF %s IS %d", parent->name, parent->no_of_children);
    printf("Inserted node type: %d\n", parent->children[parent->no_of_children-1]->type);
    printf("Inserted node: %s\n", parent->children[parent->no_of_children-1]->name);
}
//end of insert node

//flag=0 means search based on name
//flag=1 means search based on inode numder
Inode* search(Inode *node, char *name)
{
    printf("Search Node\n");
    int i;
    Inode* result;
    int no_of_children = node->no_of_children;
    printf("%s\n", name);
	printf("%d\n", no_of_children);
	if(!(strcmp(node->name,name)))
	{
		printf("NODE RETURNED\n");
		return node;
	}

	if(no_of_children == 0)
    {
        printf("NULL RETURNED\n");
		return NULL;
    }

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

			result = search(node->children[i], name);
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
    printf("NULL RETURNED\n");
        

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
    //write_to_disk_wrapper();
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
        printf("FILE SIZZE: %d STRLEN: %ld\n", inode->file.size, strlen(data));
        inode->file.data = (char*)malloc(inode->file.size * sizeof(char));
        strcpy(inode->file.data,data);
    }
    else
    {
        inode->file.size += strlen(data);
        inode->file.data = (char*)realloc(inode->file.data, inode->file.size * sizeof(char));
        strcat(inode->file.data, data);
    }
    return strlen(data);
}

//delete node
int deleteNode(const char *path)
{
    char *filename = extract_name(path);
    printf("REMOVING FILE: %s", filename);
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
    printf("rmparent: %s\n", parent->name);
    printf("rmparents children: %d\n children", parent->no_of_children);
    //write_to_disk_wrapper();
    int i, j;
    for(i=0; i<parent->no_of_children; i++)
    {
        printf("rminfor\n");
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
    /*else
    {
        parent->child_inode = (int*)realloc(parent->no_of_children, parent->no_of_children*sizeof(int));
        parent->children = (Inode**)realloc(parent->no_of_children, parent->no_of_children*sizeof(Inode*));
    }
    //int* new_child_inode = (int*)malloc(parent->no_of_children*sizeof(int));
    //int j=0;
    /*for(i=0;i<parent->no_of_children;i++)
    {
        if(parent->child_inode[i]!=temp_inum)
        {
            new_child_inode[j] = parent->child_inode[i];
            j++;
        }
        else
        {
            parent->children[i] = NULL;
        }   
    }*/
    //parent->child_inode = new_child_inode;
   //write_to_disk_wrapper();
    free(node);
    return 0; //success

}

static int sys_getattr(const char* path, struct stat *st)
{
    printf("Get Attribute Called\n");
    //Find inode of that file
    char* temp_path = path;
    printf("%s\n", path);
    char* temp_name = extract_name(temp_path);
    printf("getattr_tempname: %s\n", temp_name);
    /*if(strcmp(temp_name, ".Trash"))
        return -ENOENT;
    if(strcmp(temp_name, ".Trash-1000"))
        return -ENOENT;
    if(strcmp(temp_name, ".xdg-volume-info"))
        return -ENOENT;
    if(strcmp(temp_name, "autorun.inf"))
        return -ENOENT;*/
    Inode *I = search(root, temp_name);//found inode
    printf("Searched\n");
    if(I == NULL)
    {
        printf("ENOENT\n");
        return -ENOENT;
    }
    printf("getattr_name: %s\n", I->name);

    File F = I->file;
    if(I->type == 1)    //directory
    {
        st->st_mode = S_IFDIR | 0777;
        st->st_nlink = 2;
    }
    else if(I->type == 0)   //file
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
            printf("%s\n", I->path);

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
    write_to_disk_wrapper();
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
    write_to_disk_wrapper();
    return 0;
}

static int sys_open(const char *path, struct fuse_file_info *fi)    //return inode number
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
    write_to_disk_wrapper();
    return size;    
}

static int sys_rmdir(const char *path)
{
    printf("RMDIR CALLED for path %s\n",path);
    int ret = deleteNode(path);
    if(ret < 0)
        return -1;
    write_to_disk_wrapper();
    return 0;
}

static int sys_unlink(const char *path)
{
    printf("UNLINK CALLED for path %s\n",path);
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
        print_tree(node->no_of_children);
    }
    return;

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
    //global FILE *mfp = fopen("/home/hduser/Desktop/2.txt","w+");
    //printf("%s\n",file_blocks[inode_blocks[1].blk_no[0]].data);
    //printf("%s\n",read_data("/abc.txt"));
    //return 0; 
	/*FILE *filep = fopen("fsmeta","rb");
	if(filep==NULL)
    {
        init_root();

    }
	else
    {
        fclose(filep);
        read_from_disk_wrapper(); 
    }
    printf("TREE\n");
    print_tree(root);*/
    init_root();
    FILE *filep = fopen("fsmeta", "rb");
    if(filep != NULL)
    {
        fclose(filep);
        read_from_disk_wrapper();
    }
    return fuse_main( argc, argv, &operations);
}
