#pragma once

#include <iostream>
#include <unistd.h>
#include <string>
#include "Thread.hpp"
#include <vector>
#include <queue>
#include <functional>
#include "Log.hpp"
#include "LockGuard.hpp"

using namespace ThreadMoudle;
static const int gdefaultnum = 5;
using namespace log_ns;

void test()
{
    while (true)
    {
        std::cout << "hello wor" << std::endl;
        sleep(1);
    }
}

template <typename T>
class ThreadPool
{
private:
    void LockQueue()
    {
        pthread_mutex_lock(&_mutex);
    }

    void UnLockQueue()
    {
        pthread_mutex_unlock(&_mutex);
    }

    void Wakeup()
    {
        pthread_cond_signal(&_cond);
    }

    void WakeupAll()
    {
        pthread_cond_broadcast(&_cond);
    }

    bool IsEmpty()
    {
        return _task_queue.empty();
    }

    void Sleep()
    {
        pthread_cond_wait(&_cond, &_mutex);
    }

    void HandlerTask(const std::string &name) // this
    {
        while (true)
        {
            // 安全的取任务
            LockQueue();
            while (IsEmpty() && _isrunning)
            {
                _sleep_thread_num++;
                LOG(INFO, "%s thread sleep again!\n", name.c_str());
                Sleep();
                LOG(INFO, "%s thread wake up!\n", name.c_str());
                _sleep_thread_num--;
            }
            // 要特判的情况
            if (IsEmpty() && !_isrunning) // 哪怕线程池为false，但如果他不为空还有任务，那也不能退
            {
                UnLockQueue();
                LOG(INFO, "%s thread quit\n", name.c_str());
                break;
            }

            // 有任务 || 被唤醒
            T t = _task_queue.front();
            _task_queue.pop();
            UnLockQueue();

            t(); // 处理任务，此处不能也不用在临界区中处理
            LOG(DEBUG, "hander task done, task is : %s\n", t.result().c_str());
        }
    }

    ThreadPool(int thread_num = gdefaultnum) : _thread_num(thread_num), _isrunning(false), _sleep_thread_num(0)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond, nullptr);
    }

    ThreadPool(const ThreadPool<T> &) = delete;
    void operator=(const ThreadPool<T> &) = delete;

    void Init()
    {
        func_t func = std::bind(&ThreadPool::HandlerTask, this, std::placeholders::_1);

        for (int i = 0; i < _thread_num; i++)
        {
            std::string threadname = "thread-" + std::to_string(i);
            _threads.emplace_back(threadname, func);
            LOG(DEBUG, "construct thread %s done, init success\n", threadname.c_str());
        }
    }

    void Strat()
    {
        _isrunning = true;
        for (auto &thread : _threads)
        {
            LOG(DEBUG, "start thread %s done. \n", thread.Name().c_str());
            thread.Start();
        }
    }

public:
    void Stop()
    {
        LockQueue();

        _isrunning = false;
        WakeupAll();

        UnLockQueue();
        LOG(INFO, "thread pool stop success\n");
    }

    static ThreadPool<T> *GetInstance()
    {
        if (_tp == nullptr)
        {
            LockGuard lockguard(&_sig_mutex); // 防止多线程获取单例时 ，使进程池不是单例创建多个
            if (_tp == nullptr)
            {
                LOG(INFO, "create threadpool\n");
                _tp = new ThreadPool();
                _tp->Init();
                _tp->Strat();
            }
            else
            {
                LOG(INFO, "get threadpool\n");
            }
        }
        return _tp;
    }

    void Equeue(const T &in)
    {
        LockQueue();

        if (_isrunning)
        {
            _task_queue.push(in);
            if (_sleep_thread_num > 0)
                Wakeup();
        }

        UnLockQueue();
    }

    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }

private:
    int _thread_num;
    std::vector<Thread> _threads;
    std::queue<T> _task_queue;
    bool _isrunning;
    int _sleep_thread_num;

    pthread_mutex_t _mutex;
    pthread_cond_t _cond;

    // 单例模式
    //volatile ThreadPool<T> *_tp;
    static ThreadPool<T> *_tp;
    static pthread_mutex_t _sig_mutex;
};

template <typename T>
ThreadPool<T> *ThreadPool<T>::_tp = nullptr;

template <typename T>
pthread_mutex_t ThreadPool<T>::_sig_mutex = PTHREAD_MUTEX_INITIALIZER;