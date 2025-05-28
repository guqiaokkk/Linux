#include<iostream>
#include<string>
#include<pthread.h>
#include<unistd.h>
#include<vector>
#include<stdlib.h>
#include<thread>



void threadrun(std::string name, int num)
{
    while(num)
    {
        std::cout << name << "thread-1" << " num : " << num << std::endl;
        num--;
        sleep(1);
    }
}




int main()
{
    std::string name = "thread-1";
    std::thread mythread(threadrun, std::move(name) , 10);
    while(true)
    {
        std::cout << "main thread ... " << std::endl;
        sleep(1);
    }

    mythread.join();
    return 0;    
}


































// //可以给线程传递多个参数，甚至方法，放在结构体中
// class ThreadDate
// {
// public:
//     int Excute()
//     {
//         return x + y;
//     }
// public:
//     std::string name;
//     int x;
//     int y;
//     //other
// };

// class ThreadResult
// {
// public:
//     std::string Print()
//     {
//         return std::to_string(x) + "+" + std::to_string(y) + "=" + std::to_string(result);
//     }
// public:
//     int x;
//     int y;
//     int result;
// };



// void *threadRun(void *args)
// {
//     //std::string name = (const char*)args;
//     ThreadDate *td = static_cast<ThreadDate*>(args);
//     ThreadResult *result = new ThreadResult();
//     int cnt = 10;
//     while(cnt)
//     {
//         sleep(2);
//         std::cout << td->name << " run..."  << " ,cnt: " << cnt-- << std::endl;
//         result->result = td->Excute();
//         result->x = td->x;
//         result->y = td->y;
//         break;
//     }
//     delete td;
//     return (void*)result;
// }
// //只能有正确的返回，因为一旦异常，子线程和父线程全崩，整个进程奔溃 

//  std::string PrintToHex(pthread_t &tid)
//  {
//     char buffer[64];
//     snprintf(buffer,sizeof(buffer),"0x%lx",tid);
//     return buffer;
//  }


// void* threadrun(void *args)
// {
//     //pthread_detach(pthread_self());//把自己设为分离

//     std::string name = static_cast<const char*>(args);
//     while (true)
//     {
//         std::cout << name << " is running " << std::endl;
//         sleep(1);
//         //break;
//     }
//     //return args;
//     //exit(1);//进程退出，不能用来终止线程

//     pthread_exit(args);//专门用来终止线程 
// }



// const int num = 10;

// //main函数结束，main thread结束，表示进程结束
// int main()
// {
//     // 可以不join线程，让他执行完就退出
//     //a.一个线程被创建，默认是joinable的，必须要被join
//     //b.如果一个线程被分离，线程的工作状态分离状态，不需要也不能被join. 依旧属于进程内部，但不需要被等待了






//     //新线程如何终止
//     //1.线程函数return
//     //2.pthread_exit
//     //3.main thread call pthread_cancel, 新线程返回值是-1 


//     //创建多线程 
//     std::vector<pthread_t> tids;
//     for(int i = 0; i < num; i++)
//     {
//         //1.有线程的id
//         pthread_t tid;

//         //2. 线程的名字
//         //char name[128];   这样是错的，相当于多个线程都访问的父线程栈上的空间，要new一个然后值拷贝传递
//         //snprintf(name,sizeof(name),"thread-%d",i+1);

//         char *name = new char[128]; 
//         snprintf(name,128,"thread-%d",i+1);
//         pthread_create(&tid, nullptr, threadrun, /* 线程的名字 */name);
    
//         //3.保存所有线程的id信息    
//         //tids.push_back(tid);
//         tids.emplace_back(tid);
//     }




//     sleep(5);

//     //join todo
//     //sleep(100);
//     for(auto tid : tids)
//     {
//         //pthread_detach(tid);//主线程分离其他线程，新线程必须存在


//         pthread_cancel(tid);//   取消
//         std::cout << " cancel " << PrintToHex(tid) << std::endl;
//         //void *name = nullptr;
        
//         void *result = nullptr;//  线程被取消的退出结果是-1 
        
//         pthread_join(tid,&result);
        
//         std::cout << (long long int )result << " quit ... " << std::endl;
        
//         //std::cout << PrintToHex(tid) << " quit..." << std::endl;
//         //std::cout << (const char*)name << " quit ... " << std::endl;
//         //delete (const char*)name;
//     }



    // pthread_t tid;//pthread_t相当于unsigned long int

    // //main和new 线程不确定谁先运行 
    
    // ThreadDate *td = new ThreadDate();
    // td->name = "thread 1";
    // td->x = 10;
    // td->y = 20;

    // // ThreadDate td;
    // // td.name = "thread 1";
    // // td.num = 1;


    // int n = pthread_create(&tid, nullptr, threadRun, td);
    // if(n != 0 )// 后面可以不用关心了
    // {
    //     std::cerr << "create thread error" << std::endl;
    //     return 1;
    // }

    // //按照十六进制，将tid打印出来,他是一个虚拟地址
    // std::string  tid_str = PrintToHex(tid);
    // std::cout << "tid : " << tid_str << std::endl;

    // // 我们期望main最后退出,join来保证      子线程退了，父线程一直不退，会造成类似僵尸的问题 
    // std::cout << "main thread join begin..... " << std::endl;
    
    // //void* code = nullptr;//一个指针code，开辟了空间
    // //n = pthread_join(tid,&code);//code表示子进程的退出信息
    
    // ThreadResult *result = nullptr;
    // n = pthread_join(tid,(void**)&result);//code表示子进程的退出信息
    // if(n == 0)
    // {
    //     //std::cout << "main thread wait success , new thread exit code" << (uint64_t)code << std::endl;
    
    //     std::cout << "main thread wait success , new thread exit code" << result->Print() << std::endl;
        
    // }



//     return 0;
// }

