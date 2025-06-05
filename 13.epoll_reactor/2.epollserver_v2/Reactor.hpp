#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

#include "Connection.hpp"
#include "Epoller.hpp"

// Reactor 类似一个connection的容器，核心工作为
// 1.管理conneion和对应的内核事件
// 2.事件派发（不关心也不处理IO）
class Reactor
{
    static const int gnum = 64;

public:
    Reactor() : _epoller(std::make_unique<Epoller>()), _isrunning(false)
    {
    }

    void AddConnection(int fd, uint32_t events, const InetAddr &addr, int type)
    {
        // 1.构建一个connection
        Connection *conn = new Connection(fd);
        conn->SetEvents(events);//构建对象

        conn->SetConnectionType(type);//设置套接字类型

        conn->SetAddr(addr);//设置当前地址

        conn->SetReactor(this); // 将当前对象设置进入所有的conn对象中

        if(conn->Type() == ListenConnection)
        {
            conn->RegisterHandler(_OnConnect, nullptr, nullptr);
        }
        else
        {
            conn->RegisterHandler(_OnRecver, _OnSender, _OnExcepter);
        }


        // 2.fd和events写透到内核中，托管给epoll
        if (!_epoller->AddEvent(conn->Sockfd(), conn->Events()))
            return;

        // 3.将新的connection托管给_connections
        _connections.insert(std::make_pair(fd, conn));
    }

    void EnableConnectionReadWrite(int sockfd, bool readable, bool writeable)//开启一个连接对读，写事件关心
    {
        if(!IsConnectionExists(sockfd))
        {
            return;
        }
        
        uint32_t events = ((readable ? EPOLLIN : 0) | (writeable ? EPOLLOUT : 0) | EPOLLET);
        _connections[sockfd]->SetEvents(events);

        //写透到内核中
        _epoller->ModEvent(_connections[sockfd]->Sockfd(), _connections[sockfd]->Events());

    }

    void DelConnection(int sockfd)
    {
        //0. 安全检测
        if(!IsConnectionExists(sockfd))
        {
            return;
        } 
        LOG(INFO, "sockfd %d quit 服务器释放所有资源\n", sockfd);

        //1.在内核中移除对sockfd的关心
        EnableConnectionReadWrite(sockfd, false, false);
        _epoller->DelEvent(sockfd);

        //2.sockfd关闭
        _connections[sockfd]->Close();

        //3.在Reacotr中移除对Connection的关心
        delete _connections[sockfd];
        _connections.erase(sockfd);

    }



    void LoopOnce(int timeout) // 在Dispatcher中while
    {
        int n = _epoller->Wait(revs, gnum, timeout); // 捞取就绪事件
        for (int i = 0; i < n; i++)
        {
            int sockfd = revs[i].data.fd;
            uint32_t revents = revs[i].events;

            if (revents & EPOLLERR)
                revents |= (EPOLLIN | EPOLLOUT);
            if (revents & EPOLLHUP)
                revents |= (EPOLLIN | EPOLLOUT);
            // 一旦revents出异常出错误了，将revents转换为读写就绪，意味着把所有错误，延迟放在读写中，进行统一处理

            if (revents & EPOLLIN)
            {
                if (IsConnectionExists(sockfd) && _connections[sockfd]->_handler_recver)
                {
                    _connections[sockfd]->_handler_recver(_connections[sockfd]); // 读事件就绪，派发给对应的connection
                }
            }
            if (revents & EPOLLOUT)
            {
                if (IsConnectionExists(sockfd) && _connections[sockfd]->_handler_sender)
                {
                    _connections[sockfd]->_handler_sender(_connections[sockfd]); // 写事件就绪，派发给对应的connection
                }
            }
        }
    }

    void Dispatcher() //  事件派发
    {
        int timeout = -1;
        _isrunning = true;
        while (_isrunning)
        {
            LoopOnce(timeout);
            //other
            PrintDebug();
        }
        _isrunning = false;
    }

    bool IsConnectionExists(int sockfd)
    {
        return _connections.find(sockfd) != _connections.end();
    }

    void SetOnConnect(handler_t OnConnect)
    {
        _OnConnect = OnConnect;
    }

    void SetOnNormalHandler(handler_t recver, handler_t sender, handler_t excepter)
    {
        _OnRecver = recver;
        _OnSender = sender;
        _OnExcepter = excepter;
    }

    void PrintDebug()
    {
        std::string fdlist;
        for(auto &conn : _connections)
        {
            fdlist += std::to_string(conn.second->Sockfd()) + " ";
        }
        LOG(DEBUG, "epoll管理的fd列表:%s\n", fdlist.c_str()); 
    }

    ~Reactor()
    {
    }

private:
    // key:sockfd
    // valie:Connection*
    std::unordered_map<int, Connection *> _connections;
    std::unique_ptr<Multiplex> _epoller;
    bool _isrunning;

    struct epoll_event revs[gnum];

    // Reactor中添加处理socket的方法集
    // 1.处理新连接
    handler_t _OnConnect;
    // 2.处理普通的sockfd, 主要是IO处理
    handler_t _OnRecver;
    handler_t _OnSender;
    handler_t _OnExcepter;
};