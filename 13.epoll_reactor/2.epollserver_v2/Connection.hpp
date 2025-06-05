#pragma once


#include <iostream>
#include <string>
#include <functional>
#include "InetAddr.hpp"


#define ListenConnection 0
#define NormalConnection 1


class Connection;
class Reactor;
using handler_t = std::function<void (Connection *conn)>;

//未来我们的服务器，一切fd皆Connection，对我们来说listensockfd也是一样
class Connection
{
public:
    Connection(int sockfd): _sockfd(sockfd)
    {}

    void RegisterHandler(handler_t recver, handler_t sender, handler_t excepter)//注册方法
    {
        _handler_recver = recver; 
        _handler_sender = sender;
        _handler_excepter = excepter;
    }

    void SetEvents(uint32_t events)
    {
        _events = events;
    }

    uint32_t Events()
    {
        return _events;
    }

    void SetConnectionType(int type)
    {
        _type = type;
    }

    int Type()
    {
        return _type;
    }

    int Sockfd()
    {
        return _sockfd;
    }

    void SetReactor(Reactor *R)
    {
        _R = R;
    }

    void SetAddr(const InetAddr &addr)
    {
        _addr = addr;
    }

    void AppendInbuffer(const std::string &in)
    {
        _inbuffer += in;
    }

    void AppendOutbuffer(const std::string &in)
    {
        _outbuffer += in;
    }

    std::string &Inbuffer()
    {
        return _inbuffer;
    }

    std::string &Outbuffer()
    {
        return _outbuffer;
    }

    void DiscardOutbuffer(int n)
    {
        _outbuffer.erase(0,n);
    }

    void Close()
    {
        if(_sockfd >= 0)
            ::close(_sockfd);
    }

    ~Connection(){}


private:
    int _sockfd;
    uint32_t _events;
    std::string _inbuffer;// 我们在这里，用string来充当缓冲区
    std::string _outbuffer;//输出缓冲区
    int _type;//套接字类型

public://为了方便
    handler_t _handler_recver;  //处理读取
    handler_t _handler_sender;  //处理写入
    handler_t _handler_excepter;//处理异常  

    InetAddr _addr;

    Reactor *_R;                //定义一个Reactor的指针 . 回指向自己所属的Reactor
};