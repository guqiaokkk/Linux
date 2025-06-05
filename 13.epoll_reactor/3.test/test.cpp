#include <iostream>
#include <pthread.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <string>




//一个线程唤醒另一个线程，并且是基于文件描述符的

int evfd = -1;

void *wait(void *args)
{
    std::string name = (const char*)args;
    while (true)
    {
        uint64_t flag = 0;//8字节
        ssize_t n = ::read(evfd, &flag, sizeof(flag));
        std::cout << name << "wake up...., flag : %d:" << flag << "errno is : " << errno << std::endl;
    }
   
} 

void *wakeup(void *args)
{
    std::string name = (const char*)args;
    while (true)
    {
        errno++;//   修改errno    ------errno每个线程一份
        sleep(1);
        std::cout << "wake up one thread, errno: " << errno << std::endl;
        uint64_t flag = 1;
        ::write(evfd, &flag, sizeof(flag));
    }
    
   
}




int main()
{
    evfd = ::eventfd(0, 0);


    pthread_t tid1, tid2;
    pthread_create(&tid1, nullptr, wait, (void*)"thread-1");
    pthread_create(&tid2, nullptr, wakeup, (void*)"thread-2");



    pthread_join(tid1, nullptr);
    pthread_join(tid2, nullptr);


    ::close(evfd);
    return 0;
}