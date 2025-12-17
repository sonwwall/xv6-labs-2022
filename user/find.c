//
//Created by sonwwall on 2025/12/16
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


//适合find宝宝体质的从完整路径中取出文件名
char*
basename(char *path)
{
    char *base = path;
    char *p = path;

    while(*p != '\0'){
        if(*p == '/'){
            base = p + 1;
        }
        p++;
    }
    return base;
}

void
find(char *path,char *name){
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    //先打开一手，管你打开的是什么
    if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
    }

    //问问内核：你开了个几把
    if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
    }

    switch(st.type){
        //设备文件和普通文件，判断一下是否和提供的名称一样
        case T_DEVICE:
        case T_FILE:
            if(strcmp(basename(path),name)==0){
            printf("%s\n",path);
            break;
        }
        break;
        case T_DIR:
        //先判断一下是不是与提供的名称一样
          if(strcmp(basename(path),name)==0){
            printf("%s\n",path);
        }
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
        while(read(fd,&de,sizeof(de))==sizeof(de)){
            //无效目录项
            if(de.inum==0){
                continue;
            }
            memmove(p, de.name, DIRSIZ);//把目录项里“定长 14 字节的文件名”，原封不动拷贝到缓冲区 p 指向的位置
            p[DIRSIZ] = 0;//补充字符串结束符
            //过滤掉当前目录和父目录
            if (strcmp(p, ".") == 0 ||
                 strcmp(p, "..") == 0) {
                 continue;
                }
            //拿到子项的类型
            if(stat(buf, &st) < 0){
            printf("ls: cannot stat %s\n", buf);
            continue;
            }
            //判断一下读取的这个文件名是否匹配
            if(strcmp(p, name) == 0){
                printf("%s\n", buf);
            }

            //判断一下子项是不是目录文件
            if(st.type==T_DIR){
                find(buf,name);
            }
  
        
            
        }
        break;

    }
    close(fd);
}


int
main(int argc,char *argv[]){
    if (argc < 3){
        fprintf(2,"使用方法：find 目录 目标文件\n");
    }
    find(argv[1],argv[2]);
    exit(0);
}