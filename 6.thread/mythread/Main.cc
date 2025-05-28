#include <iostream>
#include "Thread.hpp"
#include <unistd.h>
#include <vector>
#include <cstdio>
#include "LockGuard.hpp"

using namespace ThreadMoudle;

// pthread_mutex_t gmutex = PTHREAD_MUTEX_INITIALIZER;
int tickets = 10000; // 共享资源


void route(ThreadDate *td)
{
    while (true)
    {
        LockGuard lockgurad(td->_lock);//RAII风格锁
        if (tickets > 0)
        {
            // 抢票
            usleep(1000); // 1ms ->抢票花费时间
            printf("who: %s, get a ticket: %d\n", td->_name.c_str(), tickets);
            tickets--;
        }
        else
        {
            // pthread_mutex_unlock(&gmutex);
            break;
        }
    }
}

// void route(ThreadDate *td)
// {
//     std::cout << td->_name << ":" << "mutex address: " << td->_lock << std::endl;
//     sleep(1);
//     while (true)
//     {
//         pthread_mutex_lock(td->_lock);
//         // pthread_mutex_lock(&gmutex);
//         if (tickets > 0)
//         {
//             // 抢票
//             usleep(1000); // 1ms ->抢票花费时间
//             printf("who: %s, get a ticket: %d\n", td->_name.c_str(), tickets);
//             tickets--;
//             // pthread_mutex_unlock(&gmutex);
//             pthread_mutex_unlock(td->_lock);
//         }
//         else
//         {
//             pthread_mutex_unlock(td->_lock);
//             // pthread_mutex_unlock(&gmutex);
//             break;
//         }
//     }
// }

static int threadnum = 4;

int main()
{
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr);

    std::vector<Thread> threads;
    for (int i = 0; i < threadnum; i++)
    {
        std::string name = "thread-" + std::to_string(i + 1);
        ThreadDate *td = new ThreadDate(name, &mutex);
        threads.emplace_back(name, route, td);
    }

    for (auto &thread : threads)
    {
        thread.Start();
    }

    for (auto &thread : threads)
    {
        thread.Join();
    }

    pthread_mutex_destroy(&mutex);

    // Thread t1("thread-1", route);
    // Thread t2("thread-2", route);
    // Thread t3("thread-3", route);
    // Thread t4("thread-4", route);

    // t1.Start();
    // t2.Start();
    // t3.Start();
    // t4.Start();

    // t1.Join();
    // t2.Join();
    // t3.Join();
    // t4.Join();
}

// void Print(const std::string &name)
// {
//     int cnt = 1;
//     while (true)
//     {
//         std::cout << name << " is running, cnt: " << cnt++  << std::endl;
//         sleep(1);
//     }

// }

// const int gnum = 10;

// int main()
// {
//     //管理线程，先描述，再组织
//     //构建线程对象
//     std::vector<Thread> threads;
//     for(int i = 0; i < gnum; i++)
//     {
//         std::string name = "thread-" + std::to_string(i+1);
//         threads.emplace_back(name,Print);
//         sleep(1);
//     }

//     //统一启动
//     for(auto &thread : threads)
//     {
//         thread.Start();
//     }

//     sleep(10);

//     //统一结束
//     for(auto &thread : threads)
//     {
//         thread.Stop();
//     }

//     //等待线程
//     for(auto &thread : threads)
//     {
//         thread.Join();
//     }

//     //Thread t("thread-1", Print);
//     // t.Start();

//     // std::cout << t.Name() << " ,status: " << t.Status() << std::endl;
//     // sleep(5);
//     // std::cout << t.Name() << " ,status: " << t.Status() << std::endl;

//     // t.Stop();
//     // sleep(1);
//     // std::cout << t.Name() << " ,status: " << t.Status() << std::endl;

//     // t.Join();
//     // std::cout << "join done " << std::endl;
// }