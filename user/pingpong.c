//
// Created by 瑞彤 on 2025/12/15.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc,char *argv[]){
    //先创建两个管道
    int p1[2];
    int p2[2];
    pipe(p1);
    pipe(p2);
    //fork程序
    if(fork() == 0){
        //子进程
        //关闭p1通道的发送端
        close(p1[1]);
        char c;
        //从p1中读
        int n;
        n=read(p1[0],&c,1);
        //无论是eof还是异常都不对
        if (n !=1){
            exit(1);
        }
        fprintf(2,"%d: received ping\n",getpid());
        //关闭p1通道的接收端
        close(p1[0]);

        //往p2发送
        close(p2[0]);
        n = write(p2[1],"o",1);
        if (n!=1){
            exit(1);
        }
        close(p2[1]);
        exit(0);
    }else{

        //父进程
        //关闭p1通道的接收端
        close(p1[0]);
        int n;
        n=write(p1[1],"o",1);
        if (n!=1){
            exit(1);
        }
        //关闭p1通道的发送端
        close(p1[1]);

        //接收p2通道
        close(p2[1]);
        char c;

        n=read(p2[0],&c,1);
        if (n!=1){
            exit(1);
        }
        close(p2[0]);
        fprintf(2,"%d: received pong\n",getpid());
        wait(0);
        exit(0);
    }

}