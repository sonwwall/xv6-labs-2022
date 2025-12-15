//
// Created by 瑞彤 on 2025/12/15.
//


#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc,char *argv[])
{
    //argc 是参数个数, argv 是参数内容
    if(argc <= 1){
        fprintf(2,"sleep的使用方法: sleep ticks\n");
        exit(1);
    }
    //从字符串转化为int
    int ticks;
    ticks=atoi(argv[1]);
    if(ticks == 0){
        fprintf(2,"sleep的使用方法: sleep ticks\n");
        exit(1);
    }
    fprintf(2,"嘘，系统正在睡觉\n");
    sleep(ticks);

    exit(0);
}