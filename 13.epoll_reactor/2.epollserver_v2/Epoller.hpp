#pragma once

#include <iostream>
#include <sys/epoll.h>
#include <stdlib.h>

#include "Log.hpp"
#include "Comm.hpp"

static const int gsize = 128;

using namespace log_ns;

class Multiplex
{
public:
    virtual bool AddEvent(int fd, uint32_t events) = 0;
    virtual int Wait(struct epoll_event revs[], int num, int timeout) = 0;//专门针对epoller的，struct epoll_event revs[]并非通用
    virtual bool ModEvent(int fd, uint32_t events) = 0;//修改对一个fd的关心与否
    virtual bool DelEvent(int fd) = 0;


};


class Epoller : public Multiplex
{
private:
    bool ModEventHelper(int fd, uint32_t events, int oper)
    {
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = fd;//通过这个字段，向上触发

        int n = ::epoll_ctl(_epfd, oper, fd, &ev); 
        if(n < 0)
        {
            LOG(ERROR, "epoll_ctl  %d events is %s falied:\n", fd, EventsTOString(events).c_str());
            return false;
        }
        LOG(INFO, "epoll_ctl  %d events is %s success:\n", fd, EventsTOString(events).c_str());
        return true;
    }
public:
    Epoller()
    {
        _epfd = ::epoll_create(gsize);
        if(_epfd < 0)
        {
            LOG(FATAL, "epoll_create error\n");
            exit(EPOLL_CREATE_ERROR);
        }
        LOG(INFO, "epoll_create success, epfd : %d\n", _epfd);
    }

    std::string EventsTOString(uint32_t events)
    {
        std::string eventstr;
        if (events & EPOLLIN)
            eventstr = "EPOLLIN";
        if (events & EPOLLOUT)
            eventstr += " | EPOLLOUT";
        if (events & EPOLLET)
            eventstr += " | EPOLLET";
        return eventstr;
    }




    bool AddEvent(int fd, uint32_t events) override//添加
    {
        return ModEventHelper(fd, events, EPOLL_CTL_ADD);
    }

    bool ModEvent(int fd, uint32_t events) override//修改对一个fd的关心与否
    {
        return ModEventHelper(fd, events, EPOLL_CTL_MOD);
    }

    bool DelEvent(int fd) override
    {
        return 0 == epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
    }


    int Wait(struct epoll_event revs[], int num, int timeout)
    {
        return ::epoll_wait(_epfd, revs, num, timeout);
    }




    ~Epoller()
    {}



private:
    int _epfd;

};








// class Poller : public Multiplex
// {

// };



// class Selector : public Multiplex
// {

// };