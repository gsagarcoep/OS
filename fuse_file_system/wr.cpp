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
#include <sys/vfs.h>
#include  <sys/statfs.h>
#include <utime.h>
#define MAXNODE 1000

using namespace std;

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

uid_t mount_uid;
gid_t mount_gid;
time_t mount_time;

int ind=0;

struct node
{
	int child[MAXNODE];//Stores childs of the node/directory
	int size;//number chilren
	char* path;//absolute path of node
	char* name;//name of the file
	char* content;//content in file
	uid_t uid;//owner id
	gid_t gid;//group id
	time_t mtime;//modification time
	mode_t mode;
	bool is_dir;
	nlink_t   nlink;//number of hard link 
	int id;
	bool present;//whether deleted or not
	bool symlink;
	int link[100];
};

struct node all[MAXNODE];

//Search through all nodes for the current node
int search(const char* path)
{
	//cout<<" Search "<<path<<endl;
	for (int i=0;i<ind ;i++)
	{
		if(all[i].present == false)
		{
			if(strcmp(all[i].path,"/")==0)
				all[i].present == true;
			else
				continue;
		}
		//''cout<<i<<" "<<all[i].path<<endl;
		if(strcmp(all[i].path,path)==0)
			return i;
	}
	
	return -1;
}

//Get file attributes.
static int hello_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	
	int i=search(path);
	//cout<<"get atttr --"<<path<<" "<<i<<endl;
	memset(stbuf, 0, sizeof(struct stat));
	
	if (i != -1 && (all[i].is_dir== true)) {
	//	cout<<"get atttr 2"<<endl;
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
		stbuf->st_uid=all[i].uid;
		stbuf->st_gid=all[i].gid;
		stbuf->st_atime=all[i].mtime;
		stbuf->st_mtime=all[i].mtime;
		stbuf->st_ctime=all[i].mtime;
		stbuf->st_size=4096;
	} 
	else if(i!=-1 )
	{
	//	cout<<"get atttr 1"<<endl;
		stbuf->st_mode = all[i].mode|0777;
		stbuf->st_nlink = all[i].nlink;
		stbuf->st_uid=all[i].uid;
		stbuf->st_gid=all[i].gid;
		stbuf->st_atime=all[i].mtime;
		stbuf->st_mtime=all[i].mtime;
		stbuf->st_ctime=all[i].mtime;
		stbuf->st_size=strlen(all[i].content+1);
	} 
	else
		res = -ENOENT;

	return res;
	
}

//Read directory
static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;
	//cout<<"readdir "<<endl;
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	
	int f;
	if (strcmp(path, "/") == 0) {
		f=0;
	}
	else 
	{
		f=search(path);
	}
	if(f==-1)
		return -ENOENT;
		
	for(int i=0;i<all[f].size;i++)
	{
		int j=all[f].child[i];
		if(all[j].present)
			filler(buf, all[j].name, NULL, 0);
	}
	
	return 0;
}
static int hello_opendir(const char *path, struct fuse_file_info *fi)
{
	//cout<<"opendir"<<endl;
	int f=search(path);
	if (f == -1)
		return -ENOENT;
	//cout<<"found at "<<f<<endl;
	
	if ((fi->flags & 3) != O_RDONLY)
	{
//		cout<<"not read only "<<endl;
//		return -EACCES;
	}
	//cout<<"read only "<<endl;
	return 0;
}


static int hello_open(const char *path, struct fuse_file_info *fi)
{
	cout<<"open"<<endl;
	int f=search(path);
	if (f == -1)
		return -ENOENT;
	//cout<<"found at "<<f<<endl;
	
	if ((fi->flags & 3) != O_RDONLY)
	{
//		cout<<"not read only "<<endl;
//		return -EACCES;
	}
	//cout<<"read only "<<endl;
	return 0;
}


static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;
	
	cout<<"Read "<<endl;
	int f=search(path);
	len = strlen(all[f].content);
	if(f==-1 || f==0)
		return -ENOENT;
	
	const char* temp=all[f].content;

	if (offset < len && f!= -1 && all[f].present) {
		if (offset + size > len)
			size = len - offset;

		memcpy(buf, temp + offset, size);

	} else
		size = 0;
	return size;
}

//Extract name from the path
const char * getname(const char* path)
{
	for(int j=strlen(path)-1;j>=0;j--)
	{
		if(path[j]=='/')
		{
			return (path+j+1);
		}
	}
	return NULL;
}

//Get the parent of the current node
int getparent(const char* path,char* parent)
{
	cout<<"getparent "<<endl;
	for(int j=strlen(path)-1;j>=0;j--)
	{
		if(path[j]=='/')
		{
			int i=0;
			
			if(j==0)
			{
				parent[i]=path[i];
				i++;
				parent[i]='\0';	
				continue;
			}
							
			for( i=0;i<j;i++)
			{
				parent[i]=path[i];
			
			}
			parent[i]='\0';			
			return 0;
		}
	}
	return -1;
}

/** Create a directory */
int hello_mkdir(const char *path, mode_t mode)
{
    	int f=search(path);
    	cout<<" mkdir "<<endl;
    	if(f!=-1)
		return -ENOENT;    	
    	
    	mount_uid = getuid();
	mount_gid = getgid();
	mount_time = time(NULL);
	
    	all[ind].path=new char[strlen(path)];
	strcpy(all[ind].path,path);
	all[ind].name=new char[strlen(getname(path))+1];
	strcpy(all[ind].name,getname(path));
	all[ind].content=new char[1];
	strcpy(all[ind].content,"\0");
	
	all[ind].size=0;
	all[ind].uid=mount_uid;
	all[ind].gid=mount_gid;
	all[ind].mtime=mount_time;
	all[ind].mode= mode | 0777;
	all[ind].is_dir=true;
	all[ind].nlink=2;
	all[ind].id=ind;
	all[ind].present=true;
	
	char *temp=new char[strlen(path)];
	getparent(path,temp);
	//cout<<" mkdir "<<strlen(temp)<<endl;
	int p=search(temp);
	cout<<p<<endl;
	
	if (p==-1)
		return -ENOENT;
		
	all[p].child[all[p].size]=ind++;
	all[p].size++;
	//cout<<" mkdir done "<<path<<endl;
	return 0;
}

/** Remove a directory */
int hello_rmdir(const char *path)
{
    	int f=search(path);
    	cout<<" rmdir "<<endl;
    	if(f==-1 || all[f].size!=0)
		return -ENOENT;   
		 	
    	all[f].present=false;
    	//free(all[f].name);
    	//free(all[f].path);
    	
    return 0;
}

/** Rename a file */
// both path and newpath are fs-relative
int hello_rename(const char *path, const char *newpath)
{
	int f=search(path);
    	cout<<" rename "<<endl;
    	if(f!=-1)
		return -ENOENT;    	
    	
    	//cout<<" old path "<<path<<"-- new path "<<newpath<<endl;
    	all[f].present=true;
    	delete[] all[f].name;
    	delete[] all[f].path;
    	
    	all[f].path=new char[strlen(newpath)];
	strcpy(all[f].path,newpath);
	all[f].name=new char[strlen(getname(newpath))+1];
	strcpy(all[f].name,getname(newpath));
	all[f].content=new char[1];
	strcpy(all[f].content,"\0");
	
	return 0;
}

/** Write data to an open file
 *
 * Write should return exactly the number of bytes requested
 * except on error.  An exception to this is when the 'direct_io'
 * mount option is specified (see read operation).
 *
 * Changed in version 2.2
 */
// As  with read(), the documentation above is inconsistent with the
// documentation for the write() system call.
int hello_write(const char *path, const char *buf, size_t size, off_t offset,
	     struct fuse_file_info *fi)
{
	int f=search(path);
    	cout<<"Write "<<endl;
    	if(f!=-1 && (all[f].is_dir==true))
		return -ENOENT;    	
    	
    	
    	if(f==-1)	
    	{
    		all[ind].path=new char[strlen(path)];
		strcpy(all[ind].path,path);
		
		all[ind].name=new char[strlen(getname(path))+1];
		strcpy(all[ind].name,getname(path));
		
		all[ind].content=new char[strlen(buf)+1];
		strcpy(all[ind].content,buf);
		strcat(all[ind].content,"\0");
		
		all[ind].size=0;
		all[ind].uid=getuid();
		all[ind].gid=getgid();
		all[ind].mtime=time(NULL);
		all[ind].mode=S_IFREG | 0775;
		all[ind].nlink=2;
		all[ind].id=ind;
		all[ind].present=true;
	
		char *temp=new char[strlen(path)];
		getparent(path,temp);
		int p=search(temp);
	
		if (p==-1)
			return -ENOENT;
		
		all[p].child[all[p].size]=ind++;
		all[p].size++;
    	}

	else
	{
		delete[] all[f].content;
		all[f].content=new char[strlen(buf)+1];
		strcpy(all[f].content,buf);
		strcat(all[f].content,"\0");
	}
	    	
	return strlen(buf);
}

/** Change the size of a file */
int hello_truncate(const char *path, off_t newsize)
{
	int f=search(path);
	cout<<"truncate "<<endl;
	if(f==-1 )
		return -ENOENT;
	
	char* temp=(char*)malloc(sizeof(char)*strlen(all[f].content));
	
	strcpy(temp,all[f].content);
	//free(all[f].content);
	
	all[f].content=(char*)malloc(sizeof(char)*(newsize+1));
	int i;
	for( i=0;i<newsize;i++)
	{
		all[f].content[i]=temp[i];
	
	}
	all[f].content='\0';
	if(newsize > strlen(all[f].content))
		for(int j=i;j<newsize;j++)
			all[f].content[j]='\0';
    
    	free(temp);
    
    	return 0;
}

/* Access */
int hello_access(const char *path, int mask)
{
	cout<<"Access "<<path<<endl;
	int f = search(path);
//	if (f==-1)
//		return -ENOENT;
	if((all[f].mode & mask)==mask)
		return 0;
	
  return 0;
}

/*create a file */
int hello_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    	cout<<"Create "<<endl;
    	int f=search(path);
    	
    //	if(f!=-1)
	//	return -ENOENT;    	
    	
    	if(f==-1)	
    	{
    		all[ind].path=new char[strlen(path)+1];
		strcpy(all[ind].path,path);
		//cout<<all[ind].path<<endl;
		
		all[ind].name=new char[strlen(getname(path))+1];
		strcpy(all[ind].name,getname(path));
		
		all[ind].content=new char[1];
		strcpy(all[ind].content,"\0");
		
		all[ind].size=0;
		all[ind].uid=getuid();
		all[ind].gid=getgid();
		all[ind].mtime=time(NULL);
		
		all[ind].mode=mode|777;
		all[ind].nlink=2;
		all[ind].id=ind;
		all[ind].present=true;
		all[ind].is_dir=false;
	
		char *temp=new char[strlen(path)];
		getparent(path,temp);
		int p=search(temp);
		//cout<<temp<<"Got the parent at "<<p<<endl;
		if (p==-1)
			return -ENOENT;
		
		all[p].child[all[p].size]=ind++;
		all[p].size++;
		//cout<<"Create done "<<endl;
    	}
	
    return 0;
}

//It is called while using create()  to create a file 
int hello_fgetattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi)
{
	int res = 0;
	
	int i=search(path);
	cout<<"fffget atttr  "<<endl;
	memset(stbuf, 0, sizeof(struct stat));
	
	if (i != -1 && all[i].is_dir==true) {
		//cout<<"fffget atttr 1"<<endl;
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
		stbuf->st_uid=all[i].uid;
		stbuf->st_gid=all[i].gid;
		stbuf->st_atime=all[i].mtime;
		stbuf->st_mtime=all[i].mtime;
		stbuf->st_ctime=all[i].mtime;
		stbuf->st_size=4096;
	} 
	else if(i!=-1 )
	{
	//	cout<<"fffget atttr 2"<<endl;
		stbuf->st_mode = all[i].mode|077;
	
		stbuf->st_nlink = all[i].nlink;
		stbuf->st_uid=all[i].uid;
		stbuf->st_gid=all[i].gid;
		stbuf->st_atime=all[i].mtime;
		stbuf->st_mtime=all[i].mtime;
		stbuf->st_ctime=all[i].mtime;
		stbuf->st_size=0;//strlen(all[i].content+1);
	} 
	else
		res = -ENOENT;

	return res;
}
/** Get file system statistics*/
int hello_statfs (const char *path, struct statvfs *statv)
{
	long int size=0;
	cout<<" statfs "<<endl;
	int max=0,count=0;
	
	memset(statv, 0, sizeof(struct statvfs));
	statv->f_blocks=MAXNODE; /* total data blocks in file system */
	statv->f_bfree=MAXNODE-ind; /* free blocks in fs */
	statv->f_files=ind; /* total file nodes in file system */
	statv->f_ffree=MAXNODE-ind; /* free file nodes in fs */
 	statv->f_bavail=(MAXNODE-ind)*sizeof(struct node);
	statv->f_namemax=100; /* maximum length of filenames */
	
	statv->f_bsize=sizeof(struct node);    /* file system block size */
   	statv->f_frsize=MAXNODE*sizeof(struct node);   /* fragment size */
    return 0;
}

/** Create a hard link to a file */
int hello_link(const char *path, const char *newpath)
{
    	int f=search(path);
	cout<<" link "<<endl;
	if(f==-1)// || (all[f].mode & S_IFMT)==S_IFDIR)
		return -ENOENT;    	
    	
    	char *temp=new char[strlen(path)];
	getparent(newpath,temp);
	
	int p=search(temp);
	//cout<<"I am in link"<<endl;
	if (p==-1)
		return -ENOENT;
    	
    	int nw=search(newpath);
    	
    	if(nw!=-1)
    		return -ENOENT;

	nw=ind++;
    			
    	{
    		all[nw].path=new char[strlen(newpath)];
		strcpy(all[nw].path,newpath);
		
		all[nw].name=new char[strlen(getname(newpath))+1];
		strcpy(all[nw].name,getname(newpath));
		
		all[nw].content=new char[strlen(all[f].content)+1];
		strcpy(all[nw].content,all[f].content);
		strcat(all[nw].content,"\0");
		
		all[nw].size=0;
		all[nw].uid=all[f].uid;
		all[nw].gid=all[f].uid;
		all[nw].mtime=time(NULL);
		all[nw].mode=all[f].mode|0777;
		all[nw].nlink=all[f].nlink+1;
		all[nw].id=f;
		all[nw].present=true;
		all[nw].link[all[nw].nlink-1]=f;
	
		char *temp=new char[strlen(newpath)];
		getparent(newpath,temp);
		int p=search(temp);
	//	cout<<p<<endl;
		if (p==-1)
			return -ENOENT;
		
		all[p].child[all[p].size]=nw;
		all[p].size++;
		
		all[f].link[all[f].nlink-1]=nw;
		all[f].nlink++;
	}
    	return 0;
}

/** Remove a file */
int hello_unlink(const char *path)
{
	int f=search(path);
	cout<<" unlink "<<endl;
	if(f==-1 || (all[f].is_dir==true))
		return -ENOENT;    	
    	
    	if( all[f].present == false)
    		return -ENOENT;
    		
    	all[f].present=false;
    	//delete[] all[f].content;
	//cout<<"unlink 1"<<endl;
	int len=all[f].nlink;
	if(len >1)
	{
		for(int i=0;i<len-1;i++)
		{
			int t=all[f].link[i];
			int n=all[t].nlink;
			if(n<=2)
			{
				all[t].present=false;
				continue;
			}
			
			int *temp=new int[n-1];
			for (int j=0;j<n-1;j++)
			{
				if(all[t].link[j]==f)
					continue;
				temp[j]=all[t].link[j];
			}
			all[t].nlink--;
			for (int j=0;j<n-2;j++)
			{
			
				all[t].link[j]=temp[j];
			}
		}
	}
//	cout<<"unlink 2"<<endl;	
	return 0;
}
/** Read the target of a symbolic link
 *
 * The buffer should be filled with a null terminated string.  The
 * buffer size argument includes the space for the terminating
 * null character.  If the linkname is too long to fit in the
 * buffer, it should be truncated.  The return value should be 0
 * for success.
 */

int hello_readlink(const char *path, char *link, size_t size)
{
	//cout<<link<<" read "<<size<<"link "<<path<<endl;
	int f=search(path);
	cout<<" readlink "<<endl;
	if(f==-1 )
		return -ENOENT;
	
	//cout<<" readlink "<<endl;
	
	int len=strlen(all[f].content);
	for(int i=0;i<len;i++)
	{
		if(i==size-1)
		{
			link[i]='\0';
			break;
		}
		
		link[i]=all[f].content[i];
		if(i==len-1)
		{
			link[++i]='\0';
			break;
		}
	}
	
	return 0;
}

/** Create a symbolic link */
int hello_symlink(const char *path, const char *link)
{
	cout<<path<<" symlink "<<link<<endl;
	
	char *temp=new char[strlen(link)];
	getparent(link,temp);
	
	int p=search(temp);
	
	if (p==-1)
		return -ENOENT;
    	
    	int nw=search(link);
    	
    	if(nw!=-1)
    		return -ENOENT;

	
    //	cout<<"creating new node "<<endl;
    	if(nw==-1)	
    	{
    		all[ind].path=new char[strlen(link)+1];
		strcpy(all[ind].path,link);
		
		all[ind].name=new char[strlen(getname(link))+1];
		strcpy(all[ind].name,getname(link));
		
		all[ind].content=new char[strlen(path)+1];
		strcpy(all[ind].content,path);
		
		all[ind].size=0;
		all[ind].uid=getuid();
		all[ind].gid=getgid();
		all[ind].mtime=time(NULL);
		
		all[ind].mode=S_IFLNK |0777;
		all[ind].nlink=1;
		all[ind].id=ind;
		all[ind].present=true;
	
		char *temp=new char[strlen(link)];
		getparent(link,temp);
		int p=search(temp);
	
		if (p==-1)
			return -ENOENT;
		
		all[p].child[all[p].size]=ind++;
		all[p].size++;
    //		cout<<"Done\n"<<endl;
    	}				
	return 0;    
}

/** Create a file node
 *
 * There is no create() operation, mknod() will be called for
 * creation of all non-directory, non-symlink nodes.
 */
// shouldn't that comment be "if" there is no.... ?
int hello_mknod(const char *path, mode_t mode, dev_t dev)
{
    	cout<<"makenode "<<endl;
    	int f=search(path);
    	
    	if(f!=-1)
		return -ENOENT;    	
    	
    	if(f==-1)	
    	{
    		all[ind].path=new char[strlen(path)];
		strcpy(all[ind].path,path);
		
		all[ind].name=new char[strlen(getname(path))+1];
		strcpy(all[ind].name,getname(path));
		all[ind].content=new char[1];
		strcpy(all[ind].content,"\0");
		
		all[ind].size=0;
		all[ind].uid=getuid();
		all[ind].gid=getgid();
		all[ind].mtime=time(NULL);
		
		all[ind].mode=mode|0777;
		all[ind].nlink=2;
		all[ind].id=ind;
		all[ind].present=true;
	
		char *temp=new char[strlen(path)];
		getparent(path,temp);
		int p=search(temp);
	
		if (p==-1)
			return -ENOENT;
		
		all[p].child[all[p].size]=ind++;
		all[p].size++;
    	}
    	return 0;
}
/** Change the access and/or modification times of a file */

int hello_utime(const char *path, struct utimbuf *ubuf)
{
    cout<<" utime "<<endl;
    int f=search(path);
    if(f!=-1)
    {
    	if(ubuf!=NULL)
    		all[f].mtime=ubuf->modtime;
    	all[f].mtime=time(NULL);
    }
    
    return 0;
}

//Define operations
struct tem:fuse_operations
{
	tem()
	{
		getattr	= hello_getattr;
		readdir	= hello_readdir;
		open	= hello_open;
		read	= hello_read;
		mkdir = hello_mkdir;
		rmdir = hello_rmdir;
		rename = hello_rename;
		write = hello_write;
		truncate = hello_truncate;
		access = hello_access;
		statfs = hello_statfs;
		create= hello_create;
		fgetattr = hello_fgetattr;
		link = hello_link;
		mknod = hello_mknod;
		unlink = hello_unlink;
		readlink = hello_readlink;
		symlink= hello_symlink;
		utime = hello_utime;
		opendir	= hello_opendir;
	}
};

static struct tem hello_oper;

void init ()
{
	
	mount_uid = getuid();
	mount_gid = getgid();
	mount_time = time(NULL);
	
	ind=5;
	for(int i=0;i<MAXNODE;i++)
	{
		all[i].present=false;
		all[i].symlink=false;
		all[i].is_dir=false;
	}
	
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
	all[0].mode=S_IFDIR | 0777;
	all[0].nlink=2;
	all[0].id=0;
	all[0].present=true;
	all[0].is_dir=true;
	
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
	all[1].mode=S_IFREG | 0777;
	all[1].nlink=1;
	all[1].id=1;
	all[1].present=true;

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
	all[2].mode=S_IFDIR | 0777;
	all[2].nlink=2;
	all[2].id=2;
	all[2].present=true;
	all[2].is_dir=true;
	
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
	all[3].mode=S_IFREG | 0777;
	all[3].nlink=1;
	all[3].id=1;
	all[3].present=true;
	
	all[4].path=new char[11];
	strcpy(all[4].path,"/Thisisit/hore");
	all[4].content=new char[21];
	strcpy(all[4].content,"India won the match\n ");
	all[4].size=21;
	all[4].name=new char[5];
	strcpy(all[4].name,"hore");
	all[4].uid=mount_uid;
	all[4].gid=mount_gid;
	all[4].mtime=mount_time;
	all[4].mode=S_IFREG | 0777;
	all[4].nlink=1;
	all[4].id=4;
	all[4].present=true;
	/****************************************************************/

}

int main(int argc, char *argv[])
{
	init();
	return fuse_main(argc, argv, &hello_oper, NULL);
}

