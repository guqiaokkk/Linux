#pragma once
#include<iostream>
#include<pthread.h>
#include<string>
#include<functional>
 


namespace ThreadMoudle
{


    //线程要执行的方法
    //typedef void (*func_t) (ThreadDate *td);//函数指针类型

    //等于 typedef std::funtion<void()> func_t
    using func_t = std::function<void(const std::string&)>;

    class Thread
    {
    public:
        void Excute()
        {

            _isrunning = true;
            _func(_name);
            _isrunning = false;
        }

    public:
        Thread(const std::string &name, func_t func):_name(name),_func(func)
        {
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
            }
        }

        void Join()
        {
            ::pthread_join(_tid,nullptr);
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
        
    };

}