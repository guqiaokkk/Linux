#include "RingQueue.hpp"
#include <iostream>
#include <pthread.h>
#include <ctime>
#include<unistd.h>
#include "task.hpp"


void *Consumer(void *args)
{
    Ringqueue<Task>  *rq = static_cast<Ringqueue<Task> *>(args);
    while (true)
    {
        Task t;
        //int data = 0;
        //1.消费
        rq->Pop(&t);


        //2.处理数据
        t();
        std::cout << "Consumer - > " << t.result() << std::endl;
        
    }
}

void *Productor(void *args)
{
    Ringqueue<Task>  *rq = static_cast<Ringqueue<Task> *>(args);
    
    while (true)
    {
        sleep(1); 
        //1.构造数据
        //int data = rand() % 10 + 1;
        int x = rand() % 10 + 1;
        usleep(1000*x);
        int y = rand() % 10 + 1; 
        Task t(x,y);
              
        //2.生产
        rq->Push(t);
        std::cout << "Productor - > " << t.Debug() << std::endl;
    }
    
}


int main()
{
    srand(time(nullptr)^getpid());
    Ringqueue<Task> *rq = new Ringqueue<Task>(5);

    pthread_t c,p;
    pthread_create(&c,nullptr,Consumer,rq);
    pthread_create(&p,nullptr,Productor,rq);


    pthread_join(c,nullptr);
    pthread_join(p,nullptr);
    return 0;
}