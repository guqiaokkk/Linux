#include<iostream>
#include<string>
#include<pthread.h>
#include<unistd.h>



const int num = 5;

pthread_mutex_t gmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gcond = PTHREAD_COND_INITIALIZER;







void *Wait(void* args)
{
    std::string name = static_cast<const char*>(args);
    while (true)
    {
        pthread_mutex_lock(&gmutex);

        pthread_cond_wait(&gcond,&gmutex);//这里就是线程等待的位置
        usleep(10000);
        
        std::cout << "i am : " << name << std::endl;
        
        pthread_mutex_unlock(&gmutex);
        
        //usleep(100000);
    }
    
}



int main()
{
    pthread_t threads[num];
    for(int i = 0; i < num; i++)
    {
        char *name = new char[1024];
        snprintf(name,1024,"thread-%d", i+1);
        pthread_create(threads + i,nullptr,Wait,(void*)name);
        usleep(10000);
    }

    //唤醒其他进程
    while (true)
    {
        //pthread_cond_signal(&gcond);
        pthread_cond_broadcast(&gcond); 
        std::cout << "唤醒一个线程...." << std::endl;
        sleep(2);
    }
    


    for(int i = 0; i < num; i++)
    {
        pthread_join(threads[i],nullptr);
    }
}