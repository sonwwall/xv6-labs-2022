#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


//从完整路径中取出文件名
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  //先打开一手，管你打开的是什么
  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  //问问内核：你开了个几把
  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  //分类输出
  switch(st.type){
      //设备文件和普通文件直接输出
  case T_DEVICE:
  case T_FILE:
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;
    //目录
  case T_DIR:
      //检查一下目录长度，这里超过512字节直接毙掉
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    //把路径copy到buf
    strcpy(buf, path);
    p = buf+strlen(buf);//指向path的末尾
    *p++ = '/';//指向path后一字节为"/"

    //逐个读取目录项
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)//目录是空的
        continue;//直接跳过
      memmove(p, de.name, DIRSIZ);//把目录项里“定长 14 字节的文件名”，原封不动拷贝到缓冲区 p 指向的位置
      p[DIRSIZ] = 0;//补充字符串结束符
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      //输出文件信息
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  //没有参数就列出当前目录
  if(argc < 2){
    ls(".");
    exit(0);
  }
  //有参数就逐个列出
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
