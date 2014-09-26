/*
  FUSE: Filesystem in Userspace
  
  gcc -Wall hello.c `pkg-config fuse --cflags --libs` -o hello
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

uid_t mount_uid;
gid_t mount_gid;
time_t mount_time;

int ind=0;

struct node
{
	int child[1000];
	int size;
	char* path;
	char* name;
	char* content;
	uid_t uid;
	gid_t gid;
	time_t mtime;
	mode_t mode;
	nlink_t   nlink; 
	int id;
};

struct node all[1000];

int search(const char* path)
{
	//char* temp;
	//int j;
	/*cout<<"search "<<path<<endl;
	for(j=strlen(path)-1;j>=0;j--)
	{
		if(path[j]=='/')
		{
			cout<<path+strlen(path)-j+1<<endl;
			
			break;
		}
	}	*/
	for (int i=0;i<ind;i++)
	{
		if(strcmp(all[i].path,path)==0)
			return i;
	}
	
	return -1;
}

static int hello_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	int i=search(path);
	memset(stbuf, 0, sizeof(struct stat));
	
	if (i != -1 && all[i].mode == (S_IFDIR | 0755)) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		
		stbuf->st_uid=all[i].uid;
		stbuf->st_gid=all[i].gid;
		stbuf->st_atime=all[i].mtime;
		stbuf->st_mtime=all[i].mtime;
		stbuf->st_ctime=all[i].mtime;
		stbuf->st_size=4096;
	} /*else if (strcmp(path, hello_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(hello_str);
	}
	*/else if(i!=-1 )
	{
		stbuf->st_mode = all[i].mode;
		stbuf->st_nlink = all[i].nlink;
		stbuf->st_uid=all[i].uid;
		stbuf->st_gid=all[i].gid;
		stbuf->st_atime=all[i].mtime;
		stbuf->st_mtime=all[i].mtime;
		stbuf->st_ctime=all[i].mtime;
		stbuf->st_size=strlen(all[i].content);
	} 
	else
		res = -ENOENT;

	return res;
	
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

/*	int ind=mymap[path];
	struct node* curr=all[ind];
	int child=curr->size;
*/	
/*	if (strcmp(path, "/") != 0)
		return -ENOENT;
*/
	
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	
	int f;
	if (strcmp(path, "/") == 0) {
		f=0;
	}
	else 
		f=search(path);
	if(f==-1)
		return -ENOENT;
		
	for(int i=0;i<all[f].size;i++)
	{
		int j=all[f].child[i];
		filler(buf, all[j].name, NULL, 0);
	}
	
	return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
	cout<<path<<endl;
	int f=search(path);
	if (f == -1)
		return -ENOENT;
	
	//if ((fi->flags & 3) != O_RDONLY)
	//	return -EACCES;

	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;
	/*if(strcmp(path, hello_path) != 0)
		return -ENOENT;
*/
	int f=search(path);
	len = strlen(all[f].content);
	if(f==-1 || f==0)
		return -ENOENT;
	const char* temp=all[f].content;
	if (offset < len && f!= -1) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, temp + offset, size);
	} else
		size = 0;
	cout<<size<<endl;
	return size;
}


struct temp:fuse_operations
{
	temp()
	{
		getattr	= hello_getattr;
		readdir	= hello_readdir;
		open	= hello_open;
		read	= hello_read;
	}
};

static struct temp hello_oper;

void init ()
{
	
	mount_uid = getuid();
	mount_gid = getgid();
	mount_time = time(NULL);
	
	ind=5;
	/*** For root directory ***/
	all[0].path=new char[2];
	strcpy(all[0].path,"/");
	all[0].name=new char[2];
	strcpy(all[0].name,"/");
	all[0].size=2;
	all[0].child[0]=1;
	all[0].child[1]=2;
	all[0].uid=mount_uid;
	all[0].gid=mount_gid;
	all[0].mtime=mount_time;
	all[0].mode=S_IFDIR | 0755;;
	all[0].nlink=2;
	all[0].id=0;
		
	/*** For hello ***/
	all[1].path=new char[strlen(hello_path)+1];
	strcpy(all[1].path,hello_path);
	all[1].content=new char[strlen(hello_str)];
	strcpy(all[1].content,hello_str);
	all[1].name=new char[6];
	strcpy(all[1].name,"hello");
	all[1].size=strlen(hello_str);
	all[1].uid=mount_uid;
	all[1].gid=mount_gid;
	all[1].mtime=mount_time;
	all[1].mode=S_IFREG | 0444;
	all[1].nlink=1;
	all[1].id=1;

	/*** For Thisisit ***/
	all[2].path=new char[9];
	strcpy(all[2].path,"/Thisisit");
	all[2].name=new char[9];
	strcpy(all[2].name,"Thisisit");
	all[2].size=2;
	all[2].child[0]=3;
	all[2].child[1]=4;
	all[2].uid=mount_uid;
	all[2].gid=mount_gid;
	all[2].mtime=mount_time;
	all[2].mode=S_IFDIR | 0755;;
	all[2].nlink=2;
	all[2].id=2;

	/*** Inside Thislist ***/
	all[3].path=new char[11];
	strcpy(all[3].path,"/Thisisit/nahi");
	all[3].content=new char[strlen("temporaryy thing\n")];
	strcpy(all[3].content,"temporaryy thing\n");
	all[3].size=strlen("temporaryy thing\n");
	all[3].name=new char[5];
	strcpy(all[3].name,"nahi");
	all[3].uid=mount_uid;
	all[3].gid=mount_gid;
	all[3].mtime=mount_time;
	all[3].mode=S_IFREG | 0444;
	all[3].nlink=1;
	all[3].id=1;
	
	all[4].path=new char[11];
	strcpy(all[4].path,"/Thisisit/hore");
	all[4].content=new char[21];
	strcpy(all[4].content,"India won the match ");
	all[4].size=21;
	all[4].name=new char[5];
	strcpy(all[4].name,"hore");
	all[4].uid=mount_uid;
	all[4].gid=mount_gid;
	all[4].mtime=mount_time;
	all[4].mode=S_IFREG | 0444;
	all[4].nlink=1;
	all[4].id=4;
	/****************************************************************/

}

int main(int argc, char *argv[])
{
	init();
	return fuse_main(argc, argv, &hello_oper, NULL);
}

