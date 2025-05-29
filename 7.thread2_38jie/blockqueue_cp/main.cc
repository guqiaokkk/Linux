#include "BlockQueue.hpp"
#include<pthread.h>
#include<ctime>
#include<unistd.h>
#include "task.hpp"

void *Consumer(void* args)
{
    BlockQueue<task_t> *bq = static_cast<BlockQueue<task_t> *>(args);
    while (true)
    {
        //sleep(2);
        //1.获取数据
        //int data = 0;
        task_t t;
        bq->Pop(&t);

        //2.处理数据
        //t.Excute();
        t();
        //std::cout << "Consumer-> " << t.result() << std::endl;
    }
    
}

void *Productor(void* args)
{
    srand(time(nullptr) ^ getpid());
    BlockQueue<task_t> *bq = static_cast<BlockQueue<task_t> *>(args);
    while (true)
    {
        sleep(2);
        //1.构建数据/任务
        //int data = rand() % 10  + 1;
        // int x = rand() % 10  + 1;
        // usleep(x * 1000);
        // int y = rand() % 10  + 1;
        // Task t(x,y);
        //2.生产数据
        bq->Equeue(DownLoad);
        //std::cout << "Productor-> " << t.Debug() << std::endl;
        std::cout << "Productor->  Download" << std::endl;
    }
    
}



int main()
{
    BlockQueue<task_t> *bq = new BlockQueue<task_t>();
    pthread_t c,p;

    pthread_create(&c,nullptr,Consumer,bq);
    pthread_create(&p,nullptr,Productor,bq);

    pthread_join(c,nullptr);
    pthread_join(p,nullptr);

    return 0;
}