#pragma once
#include<iostream>
#include<ctime>
#include<cstdlib>
#include<sys/types.h>
#include<unistd.h>

#define TaskNum 3

typedef void (*task_t) ();//定义了一个task_t函数指针类型，返回值为void,参数为空的函数指针

task_t tasks[TaskNum];

void print()
{
    std::cout << "i am print" << std::endl;
}
void download()
{
    std::cout << "i am download" << std::endl;
}
void flush()
{
    std::cout << "i am flush" << std::endl;
}


void LoadTask()
{
    srand(time(nullptr) ^ getpid());
    tasks[0] = print;
    tasks[1] = download;
    tasks[2] = flush;
}


void ExcuteTask(int number)
{
    if( number < 0 || number > 2)return;
    tasks[number]();
}

int SelectTask()
{
    return rand() % TaskNum ;
}


void work()//如果使用/dup2(pipefd[0],0)的话
{
    while (true)
    {
        int command = 0;
        int n = read(0,&command, sizeof(command));
        // read 从文件描述符 rfd 中读取数据，并将其存储到 command 的内存中。
        if (n == sizeof(int))
        {
            ExcuteTask(command);
        }
        else if(n == 0)
        {
            std::cout << "sub process:" << getpid() << "quit" << std::endl;
            break;
        }
    }
}