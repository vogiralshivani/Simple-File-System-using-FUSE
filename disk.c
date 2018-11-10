#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int no_of_blocks = INISIZE/BLKSIZE;
int inode_bitmap[no_of_blocks];
int data_bitmap[no_of_blocks];
File fblocks[no_of_blocks];
Inode inodeblocks[no_of_blocks];



char *filename = "fsdata";
void write_to_disk_wrapper()
{
	
	FILE *fp = fopen(filename,"rb");
	Inode *temp = root;

	//writing root node
	inode_bitmap[0] = 1;
	data_bitmap[0] = 1;
	fblocks[0]->data = temp->file->data;
	fblocks[0]->size = temp->file->size;
	fwrite(inode_bitmap,sizeof(inode_bitmap),1,fp);
	fwrite(data_bitmap,sizeof(data_bitmap),1,fp);
	fwrite(fblocks,sizeof(fblocks),1,fp);
	int j;
	int n=temp->no_of_children;
	for(j=1;j<n;i++)
	{
		write_to_disk(temp->children[j]);
	}
}

void write_to_disk(Inode *node)
{
	static int i=1;
	if(node->type==0)
	{
		inode_bitmap[i] = 1;
		data_bitmap[i] = 1;
		fblocks[i]->data = node->file->data;
		fblocks[i]->size = node->file->size;
		fwrite(inode_bitmap,sizeof(inode_bitmap),1,fp);
		fwrite(data_bitmap,sizeof(data_bitmap),1,fp);
		fwrite(fblocks,sizeof(fblocks),1,fp);
		i++;
		return;
	}	
	else
	{
		int j=0;
		for(j=0;i<node->no_of_children;j++)
		{
			write_to_disk(node->children[i]);
		}
	}
	return;
}
