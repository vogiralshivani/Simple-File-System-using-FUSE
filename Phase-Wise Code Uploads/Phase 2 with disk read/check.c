#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "check.h"

void read_from_datafile(meta_node*disk)
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
void read_from_disk(meta_node* disk, FILE *mfp)
{
	int i;
    fread(&disk->pathlength, sizeof(disk->pathlength), 1, mfp);
    printf("%ld\n", sizeof(disk->pathlength));
    printf("Pathlen read %d\n", disk->pathlength);
   	printf("FILE POINTER: %ld\n", ftell(mfp));

    disk->path = (char*)malloc(disk->pathlength*sizeof(char)+1);
    fread(disk->path, sizeof(disk->path), 1, mfp);
    printf("%ld\n", sizeof(disk->path));
    printf("Path read %s\n", disk->path);
    printf("FILE POINTER: %ld\n", ftell(mfp));

    fread(&disk->namelength, sizeof(disk->namelength), 1, mfp);
    printf("%ld\n", sizeof(disk->namelength));
    printf("Namelen read %d\n", disk->namelength);
    printf("FILE POINTER: %ld\n", ftell(mfp));

    disk->name = (char*)malloc(disk->namelength*sizeof(char)+1);
    fread(disk->name, sizeof(disk->name), 1, mfp);
    printf("%ld\n", sizeof(disk->name));
    printf("Name read %s\n", disk->name);
    printf("FILE POINTER: %ld\n", ftell(mfp));
    
    fread(&disk->type, sizeof(disk->type), 1, mfp);
    printf("%ld\n", sizeof(disk->type));
	printf("Type read %d\n", disk->type);
	printf("FILE POINTER: %ld\n", ftell(mfp));
    
    fread(&disk->no_of_children, sizeof(disk->no_of_children), 1, mfp);
    printf("%ld\n", sizeof(disk->no_of_children));
    printf("No_children read %d\n", disk->no_of_children);
	printf("FILE POINTER: %ld\n", ftell(mfp));
    
    fread(&disk->size, sizeof(disk->size), 1, mfp);
    printf("%ld\n", sizeof(disk->size));
    printf("Size read %d\n", disk->size);
	printf("FILE POINTER: %ld\n", ftell(mfp));

    fread(&disk->inode_num, sizeof(disk->inode_num), 1, mfp);
    printf("%ld\n", sizeof(disk->inode_num));    
    printf("Inode_num read %d\n", disk->inode_num);
	printf("FILE POINTER: %ld\n", ftell(mfp));
    
    if(disk->no_of_children == 0)
    	disk->child_inode = NULL;
    else
    {
    	disk->child_inode = (int*)malloc(disk->no_of_children*sizeof(int));
    	printf("Size of child inode%ld\n", sizeof(disk->child_inode));
        fread(disk->child_inode, sizeof(disk->child_inode), 1, mfp);
        for(i=0; i<2; i++)
        	printf("Child_inode read %d\n", disk->child_inode[i]);
   	}
   	printf("FILE POINTER: %ld\n", ftell(mfp));
    
    fread(&disk->parent_inode, sizeof(disk->parent_inode), 1, mfp);
    printf("%ld\n", sizeof(disk->parent_inode));    
    printf("Parent read %d\n", disk->parent_inode);
	printf("FILE POINTER: %ld\n", ftell(mfp));
    
    fread(&disk->file.size, sizeof(disk->file.size), 1, mfp);
    printf("File size read %d\n", disk->file.size);
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
    printf("SIZE OF DISK %ld\n", sizeof(disk));
    printf("%ld\n", ftell(mfp));
    printf("Filedata read\n");
	//meta_node has been modified now, we want the Inode to build the tree. 
	Inode *new_node = (Inode*)malloc(sizeof(Inode));
	new_node->path = disk->path;
	new_node->name = disk->name;
	new_node->parent_name = disk->parent_name;
	new_node->type = disk->type;
	new_node->no_of_children = disk->no_of_children;
	new_node->size = disk->size;
	new_node->inode_num = disk->inode_num;
	new_node->child_inode = disk->child_inode;
	Inode *pnode = search(root,new_node->parent_name,-1,0);
	new_node->parent = pnode;
	for(i =0;i<new_node->no_of_children;i++)
	{
		new_node->children[i]=search(root,NULL,new_node->child_inode[i],1);
	}
	new_node->file.data = disk->file.data;
    new_node->file.size = disk->file.size;
	insert_node(new_node);
	
}
void main()
{
	FILE *mfp;
	mfp = fopen("fsmeta","rb");
	printf("here\n");
	meta_node *temp = (meta_node*)malloc(sizeof(meta_node));
	int tmp;
	while(feof(mfp) == 0)
	{
		tmp = fgetc(mfp);
		printf("in while\n");
		//fread(temp,sizeof(meta_node),1,mfp);
		if(tmp!=EOF)
		{
			ungetc(tmp, mfp);
			read_from_disk(temp, mfp);
		}
		printf("fin read\n");
		printf ("inode_num = %s\n", (temp->name)); 

	}
	free(temp);
	fclose(mfp);
}
