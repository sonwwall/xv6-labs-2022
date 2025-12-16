//
// Created by 瑞彤 on 2025/12/15.
//


#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

__attribute__((noreturn))
void
filter(int read_fd){
    int f;
    int r=read(read_fd,&f,4);
    if (r==4){
        printf("prime %d\n",f);
        int p[2];
        pipe(p);
        if(fork()==0){
            //子进程
            close(read_fd);
            close(p[1]);//关闭发送端
            filter(p[0]);
        }else{
            //父进程
            close(p[0]);//关闭接收端
            while(1){
                int n;
                r=read(read_fd,&n,4);
                if(r==4){
                    if(n%f!=0){
                        write(p[1],&n,4);
                    }
                }
                if(r==0){
                    close(p[1]);
                    close(read_fd);
                    wait(0);
                    exit(0);
                }
            }
        }
    }
    if(r==0){
        close(read_fd);
        exit(0);
    }else{
        exit(1);
    }
}


int
main(int argc, char *argv[]){
    int p1[2];
    pipe(p1);
    if(fork()==0){
        //子进程
        close(p1[1]);
        filter(p1[0]);

    }else{
        //父进程
        close(p1[0]);
        for(int i=2;i<=35;i++){
            int r=write(p1[1],&i,4);
            if (r==-1){
                exit(1);
            }
        }
        close(p1[1]);
        wait(0);
        exit(0);
    }

}