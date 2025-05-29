#pragma once
#include<iostream>
#include<functional>

// using task_t = std::function<void()>; // 等价于typedef std::function<void()> task_t; 

// void DownLoad()
// {
//     std::cout << "我是一个下载的任务...." << std::endl;
// }

//做一个加法

class Task
{
public:
    Task()
    {}
    Task(int x,int y):_x(x),_y(y)
    {}

    void Excute()
    {
        _result =  _x + _y;
    }

    void operator()()
    {
        Excute();
    }
    
    std::string Debug()
    {
        std::string msg = std::to_string(_x) + "+" + std::to_string(_y) + "= ?";
        return msg;
    }
    std::string result()
    {
        std::string msg = std::to_string(_x) + "+" + std::to_string(_y) + "= " + std::to_string(_result);
        return msg;
    }
    
    
    ~Task()
    {

    }

    
private:
    int _x;
    int _y;
    int _result;
};