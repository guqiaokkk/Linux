#include<iostream>
#include<unistd.h>
#include<cstdio>
#include<sys/types.h>
#include<sys/wait.h>


void PrintPending(sigset_t &pending)
{
    std::cout << "curr process[ " << getpid() << "]pending :";
    for(int signo = 31; signo >= 1; signo--)
    {
        if(sigismember(&pending,signo))std::cout << 1 ;
        else std::cout << 0;
    }
    std::cout << "\n";
}



int main()
{
    //0.屏蔽二号信号
    sigset_t block_set,old_set;
    sigemptyset(&block_set);//清空
    sigemptyset(&old_set);

    sigaddset(&block_set,2);//添加信号
    //1.1 设置进入进程的Block表中
    sigprocmask(SIG_BLOCK,&block_set,&old_set);//修改了当前进程内核的block表，完成了对2号信号的屏蔽
    
    int cnt = 10;
    while(true)
    {
        //2.获取当前进程的pending信号集
        sigset_t pending;
        sigpending(&pending);

        //3.打印
        PrintPending(pending);
        cnt--;

        //4.解除对2号信号的屏蔽
        if(cnt == 0)
        {
            std::cout << "解除对2号信号的屏蔽!!!" << std::endl;
            sigprocmask(SIG_SETMASK,&old_set,&block_set);
        }
        sleep(1);
    }
}   






// int Sum(int start,int end)
// {
//     int sum = 0;
//     for(int i = start; i < end; i++)
//     {
//         sum /= 0; //core
//         sum += i;
//     }
//     return sum;
// }



// int main()
// {
//     pid_t id = fork();
//     if(id == 0)
//     {
//         sleep(1); 
//         //child
//         Sum(0,100);
//         exit(0);
//     }
//     //father
//     int status = 0;
//     pid_t rid = waitpid(id,&status,0);
//     if(rid == id)
//     {
//         printf("exit code : %d, exit sig : %d, core dump : %d\n", (status>>8)&0xFF,status&0x7F,(status>>7)&0x1);
//     }
//     return 0;
// }