#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "InetAddr.hpp"
#include "ThreadPool.hpp"
#include "LockGuard.hpp"

using task_t = std::function<void()>;

class Route
{
public:
    Route()
    {
        pthread_mutex_init(&_mutex, nullptr);
    }
    void CheckOnlineUser(InetAddr &who)
    {
        LockGuard Lockguard(&_mutex);
        for(auto &user : _online_user)
        {

            if(user == who)
            {
                LOG(DEBUG, "%s is exists\n", who.AddStr().c_str());
                return;
            }
        }
        LOG(DEBUG, "%s is not exists, add it\n", who.AddStr().c_str());
        _online_user.push_back(who);
    }

    void Offline(InetAddr &who)
    {
        LockGuard Lockguard(&_mutex);
        auto iter = _online_user.begin();
        for( ;iter != _online_user.end(); iter++)
        {
            if(*iter == who)
            {
                LOG(DEBUG, "%s is offline\n", who.AddStr().c_str());
                _online_user.erase(iter);
                break;
            }
        }
    }

    void ForwardHelper(int sockfd, const std::string message, InetAddr who)
    {
        LockGuard Lockguard(&_mutex);
        std::string send_message = "[" + who.AddStr() + "]#" + message;
        for(auto &user : _online_user)
        {
            struct sockaddr_in peer = user.Addr();
            LOG(DEBUG, "Forward message to %s, message is %s\n", user.AddStr().c_str(), send_message.c_str());
            ::sendto(sockfd, send_message.c_str(), send_message.size(), 0, (struct sockaddr*)&peer, sizeof(peer));
        }
    }

    void Forward(int sockfd, const std::string &message, InetAddr &who)
    {  
        //1.判断用户是否在用户列表里，若在则什么都不做，若不在则把他加入到列表中
        CheckOnlineUser(who);

        //1.1 message == "QUIT || Q"
        if(message == "QUIT" || message == "Q") 
        {
            Offline(who);
        }

        //2.此时who一定在列表里
        //ForwardHelper(sockfd, message);
        task_t t = std::bind(&Route::ForwardHelper, this, sockfd, message, who);
        ThreadPool<task_t>::GetInstance()->Equeue(t);
    }
    ~Route()
    {
        pthread_mutex_destroy(&_mutex);
    }
private:
    std::vector<InetAddr> _online_user;
    pthread_mutex_t _mutex;
};