#pragma once
#include<iostream>
#include<pthread.h>
#include<string>

 


namespace ThreadMoudle
{
    class ThreadDate
    {
    public:
        ThreadDate(const std::string &name, pthread_mutex_t *lock):_name(name),_lock(lock)
        {

        }
    public:
        std::string _name;
        pthread_mutex_t *_lock;
    };



    //线程要执行的方法
    typedef void (*func_t) (ThreadDate *td);//函数指针类型

    class Thread
    {
    public:
        void Excute()
        {
            std::cout <<  _name << " is running " << std::endl;

            _isrunning = true;
            _func(_td);
            _isrunning = false;
        }

    public:
        Thread(const std::string &name, func_t func, ThreadDate *td):_name(name),_func(func),_td(td)
        {
            std::cout << "create " << name << "done " << std::endl;
        }

        //不static的话，他默认有个this参数
        static void* ThreadRoutine(void *args)//新线程都会执行该方法
        {
            Thread *self = static_cast<Thread*>(args);//获得了当前对象
            self->Excute(); 
            return nullptr; 
        }

        bool Start()
        {
            int n = ::pthread_create(&_tid,nullptr,ThreadRoutine,this);
            if(n != 0)return false;     
            return true;
        }       

        const std::string Status()
        {
            if(_isrunning) return "running";
            else return "sleep";
        }

        void Stop()
        {
            if(_isrunning == true)
            {
                ::pthread_cancel(_tid);
                _isrunning = false;
                std::cout <<  _name << " stop " << std::endl;
            }
        }

        void Join()
        {
            ::pthread_join(_tid,nullptr);
            std::cout << _name << " Joined " << std::endl;
        }


        std::string Name()
        {
            return _name;
        }

        ~Thread()
        {

        }


    private:
        std::string _name;
        pthread_t _tid;
        bool _isrunning;
        func_t _func;//线程要执行的回调函数
        ThreadDate *_td;
    };

}