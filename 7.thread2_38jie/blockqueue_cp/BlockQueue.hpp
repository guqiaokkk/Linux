#pragma once

#include<iostream>
#include<string>
#include<queue>
#include<pthread.h>


const static int defaultcap = 5;


template <typename T>
class BlockQueue
{
private:
    bool IsFull()
    {
        return _block_queue.size() == _max_cap;
    }

    bool IsEmpty()
    {
        return _block_queue.empty();
    }


public:
    BlockQueue(int cap = defaultcap):_max_cap(cap)
    {
        pthread_mutex_init(&_mutex,nullptr);
        pthread_cond_init(&_p_cond,nullptr);
        pthread_cond_init(&_c_cond,nullptr);
    }
    
    void Pop(T *out)
    {
        pthread_mutex_lock(&_mutex);
        while(IsEmpty())//if不能做到,添加尚未满足，但是进程被异常唤醒，叫做伪唤醒
        {
            pthread_cond_wait(&_c_cond,&_mutex); //如果有两个消费者都在这里等，生产者生产了一个数据，然后用的是broadcast，两个进程都被唤醒，
                                                 //一个进程争到锁，一个进程在mutex处等不在条件变量下等，此时有问题的，所以不能用if要用while
        }

        //1.没有空 || 2.被唤醒
        *out = _block_queue.front();
        _block_queue.pop(); 
        pthread_mutex_unlock(&_mutex);      
        //让生产者生产
        pthread_cond_signal(&_p_cond);         //if(_block_queue.size() < lower_water)pthread_cond_signal(&_p_cond);
    }
    
    void Equeue(const T &in)
    {
        pthread_mutex_lock(&_mutex);
        while(IsFull())//while可以保证代码的鲁棒性（健壮性）  
        {
            //满了，生产者不能生产了，必须等待
            //虽然此时在临界区里，但pthread_cond_wait    被调用的时候：除了让自己继续排队等待，还会自己释放传入的锁
            // 因为函数返回的时候，还要在临界区里，所以返回时：必须先参与锁的竞争，重新加上锁，该函数才会返回           
            pthread_cond_wait(&_p_cond,&_mutex);
        }
        
        //1.没有满 || 2.被唤醒
        _block_queue.push(in);//生产到阻塞队列    
        pthread_mutex_unlock(&_mutex);          
        //让消费者消费
        pthread_cond_signal(&_c_cond);     //if(_block_queue.size() > high_water)pthread_cond_signal(&_c_cond);
    }
    
    
    
    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_p_cond);
        pthread_cond_destroy(&_c_cond);
    }



private:
    std::queue<T> _block_queue;//将他维护成临界资源
    int _max_cap;
    pthread_mutex_t _mutex;
    pthread_cond_t _p_cond;//生产者等待条件变量
    pthread_cond_t _c_cond;//消费者等待条件变量
 
};