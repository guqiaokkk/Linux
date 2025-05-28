#include<iostream>
#include<string>
#include<pthread.h>
#include<unistd.h>
#include<cstdio>



 //linux里有效  __thread只能修饰内置类型
__thread int gval = 100;//线程级别的全局变量，有多少个线程就有多少个gval 

std::string ToHex(pthread_t tid)
{
    char id[128];
    snprintf(id,sizeof(id),"0x%lx",tid);
    return id;
}




void* threadrun(void *args)
{
    std::string name = static_cast<const char*>(args);
    while (true)
    {
        std::string id =  ToHex(pthread_self());
        std::cout << name << " is running, tid: " << id << " , gavl: " << gval << " , &gavl" << &gval << std::endl;
        gval++;
        sleep(1); 
    }
    
}



 

int main()
{
    pthread_t tid;//线程属性集合的起始虚拟地址 -- 在pthread库中维护
    pthread_create(&tid,nullptr,threadrun,(void*)"thread-1");
    
    while (true)
    {
        std::cout << "main thread gval: " << gval << " , &gavl" << &gval << std::endl; 
        sleep(1);
    }
    
    
    
    
    
    
    pthread_join(tid,nullptr);
    return 0;
}