#include "ThreadPool.hpp"
#include "task.hpp"
#include "Log.hpp"

//#include<memory>

using namespace log_ns;

int main()
{
    EnableScreen();
    //std::unique_ptr<ThreadPool> tp = std::make_unique<ThreadPool>();
    // ThreadPool<Task> *tp = new ThreadPool<Task>();
    
    // tp->Init();
    // tp->Strat();

    int cnt = 10;
    while (cnt--)
    {
        //不断地向线程池推送任务
        sleep(1)    ;
        Task t(1,1);
        ThreadPool<Task>::GetInstance()->Equeue(t);
        LOG(INFO,"equeue a task,%s\n",t.Debug().c_str());
        sleep(1);
    }

    ThreadPool<Task>::GetInstance()->Stop();
    LOG(INFO,"thread pool stop!\n");
    return 0;
    
}