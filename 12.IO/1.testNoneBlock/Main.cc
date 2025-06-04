#include "Comm.hpp"


#include <iostream>
#include <unistd.h>
#include <cstdio>

#include <sys/select.h>

int main()
{
    char buffer[1024];
    SetNonBlock(0);
    while (true)
    {
        //printf("Enter# ");
        //fflush(stdout);
        ssize_t n = ::read(0,buffer,sizeof(buffer)-1);
        if(n > 0)
        {
            buffer[n] = 0;
            printf("echo# %s", buffer);
        }
        else if(n == 0)//ctrl + d
        {
            printf("read done \n");
            break;
        }
        else{
            // perror("read\n");//如果是非阻塞，底层数据未就绪，IO接口会以出错形式返回
            // printf("n = %ld\n", n);
           
            //根据errno错误码，区分底层未就绪还是真的出错了
            if(errno == 11)//底层数据没有就绪，errno就会被设置成EAGAIN（11)
            {
                sleep(1);
                std::cout << "i want again,底层数据没有就绪，开始轮询检测" << std::endl;
                std::cout << "可以做其他事情" << std::endl;
                //do other thing
                continue;
            }

            else if(errno == EINTR)//被信号中断了
            {
                continue;
            }

            else{//真正的错误 
                perror("read");
                break;
            }
           
            // printf("error = %d\n", errno);
            // break;
        }
    }
    

    return 0;
}