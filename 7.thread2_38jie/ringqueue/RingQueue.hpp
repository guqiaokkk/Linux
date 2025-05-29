#pragma once 

#include<iostream>
#include<string>
#include<vector>
#include<semaphore.h>
#include <pthread.h>


template<typename T>
class Ringqueue
{
private:
    void P(sem_t &s)//对信号量进行申请
    {
        sem_wait(&s);
    }

    void V(sem_t &s)//对信号量释放
    {
        sem_post(&s);   
    }


public:
    Ringqueue(int max_cap)
    :_max_cap(max_cap),_ringqueue(max_cap),_c_step(0),_p_step(0)
    {
        sem_init(&_data_sem,0,0);
        sem_init(&_space_sem,0,max_cap);

        pthread_mutex_init(&_c_mutex,nullptr);
        pthread_mutex_init(&_p_mutex,nullptr);
    }

    void Push(const T &in)//生产者 
    {
        //信号量：是一个计数器，是资源的预订机制。预订：在外部，可以不判断资源是否满足，就可以知道内部资源的情况
        P(_space_sem);
        //先申请信号量再申请锁
     
        pthread_mutex_lock(&_p_mutex);
        _ringqueue[_p_step] = in;
        _p_step++;
        _p_step %= _max_cap;
        pthread_mutex_unlock(&_p_mutex);
        V(_data_sem);
    }

    void Pop(T *out)//消费
    {   
        P(_data_sem);

        pthread_mutex_lock(&_c_mutex);
        *out = _ringqueue[_c_step];
        _c_step++;
        _c_step %= _max_cap;
        pthread_mutex_unlock(&_c_mutex);
        V(_space_sem);
    }



    ~Ringqueue()
    {
        sem_destroy(&_data_sem);
        sem_destroy(&_space_sem);

        pthread_mutex_destroy(&_c_mutex);
        pthread_mutex_destroy(&_p_mutex);
    }

private:
    std::vector<T> _ringqueue;
    int _max_cap;

    int _c_step;
    int _p_step;

    sem_t _data_sem;//消费者关心的
    sem_t _space_sem;//生产者关心的

    pthread_mutex_t _c_mutex;
    pthread_mutex_t _p_mutex;
};