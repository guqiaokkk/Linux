#include<iostream>
#include<unistd.h>
#include<ctime>

int gval = 100;


//新线程   两个线程各分到一些虚拟地址
void* threadStart(void* args)
{
    while (true)
    {
        //int x = rand() % 5;
        std::cout << "new thread running ..." <<" ,pid : "<< getpid() 
        << " gval  "<< gval << ", &gavl" << &gval << std::endl;
        sleep(1);
     
        // if(x == 0)
        // {
        //     int *p = nullptr;
        //     *p = 100;//故意写了野指针
        // }
    }
    
}




int main()
{
    srand(time(nullptr));

    pthread_t tid1;
    pthread_create(&tid1,nullptr,threadStart,(void*)"thread-new");

    pthread_t tid2;
    pthread_create(&tid2,nullptr,threadStart,(void*)"thread-new");

    pthread_t tid3;
    pthread_create(&tid3,nullptr,threadStart,(void*)"thread-new");



    //主线程
    while (true)
    {
        std::cout << "main thread running ..." << " ,pid: "<< getpid()
        << " gval  "<< gval << ", &gavl" << &gval << std::endl;

        gval++;//修改

        sleep(1);

    }
    

    return 0;
}