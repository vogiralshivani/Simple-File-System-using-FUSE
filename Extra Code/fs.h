#include <stdio.h>
#include <fuse.h>

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

#define INISIZE 1048576	//INIT_SIZE
#define BLKSIZE 4096	//BLOCK_SIZE

char *reverse(char *str, int mode);
char *extractDir(char *path);
char *extract_name(char *path);
void init_root();
void init_node(const char * path, char * name, Inode *parent,int type);
void insert_node(Inode *node);
Inode* search(Inode *node, char *name);
char * read_data(Inode* node);
int write_data(Inode* inode,char *data);

static int sys_getattr(const char* path, struct stat *st);
static int sys_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi );
static int sys_mkdir(const char * path, mode_t x);
static int sys_mknod(const char * path, mode_t x, dev_t y);
static int sys_open(const char *path, struct fuse_file_info *fi);	//return inode number
static int sys_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi );
static int sys_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

static struct fuse_operations operations = {
    .getattr	= sys_getattr,
    .readdir	= sys_readdir,
    .read		= sys_read,
    .mknod      = sys_mknod,
    .write      = sys_write,
    .mkdir      = sys_mkdir,
    .open       = sys_open,
//    .rmdir      = sys_rmdir,
//    .unlink     = do_unlink,
};

Inode *root;