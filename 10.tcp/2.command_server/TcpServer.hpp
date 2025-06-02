#pragma once

#include <iostream>
#include <cstring>
#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>

#include "Log.hpp"
#include "InetAddr.hpp"




const static int gport = 8888;
const static int gsock = -1;
const static int gblcklog = 8;

enum 
{
    SOCKET_ERROR = 1,
    BIND_ERROR,
    LISTEN_ERROR
};

using namespace log_ns;



using command_service_t = std::function<void(int sockfd, InetAddr addr)>;

class TcpServer
{
public:
    TcpServer(command_service_t service,uint16_t port = gport)
    :_port(port),
    _listensockfd(gsock),
    _isrunning(false),
    _service(service)
    {}
    
    void InitServer()
    {
        //1.创建socket
        _listensockfd = ::socket(AF_INET, SOCK_STREAM, 0);
        if(_listensockfd < 0)
        {
            LOG(FATAL, "socket create error\n");
            exit(SOCKET_ERROR);
        }
        LOG(INFO, "socket create success, sockfd: %d\n", _listensockfd);//3

        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));

        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;
    
        //2.bind sockfd 和 Socket addr
        if(::bind(_listensockfd, (struct sockaddr*)&local, sizeof(local)) < 0)
        {
            LOG(FATAL, "bind error\n");
            exit(BIND_ERROR);
        }
        LOG(INFO, "bind success\n");

        //3.TCP面向连接，TCP需要能不断获取连接
        if(::listen(_listensockfd, gblcklog) < 0)
        {
            LOG(FATAL, "lieten error\n");
            exit(LISTEN_ERROR);
        }
        LOG(INFO, "listen success\n");
    }
    
    class ThreadData
    {
    public:
        int _sockfd;
        TcpServer *_self;
        InetAddr _addr;
    public:
        ThreadData(int sockfd, TcpServer* self, const InetAddr &addr):_sockfd(sockfd),_self(self),_addr(addr)
        {}
    };

    void Loop()
    {
        _isrunning = true;
        while (_isrunning)
        {
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            //4.获取新连接
            int sockfd = ::accept(_listensockfd,(struct sockaddr*)&client, &len);
            if(sockfd < 0)
            {
                LOG(WARNING,"accept error\n");
                continue;
            }
            InetAddr addr(client);
            LOG(INFO, "get a new link, client info: %s, sockfd is : %d\n",addr.AddStr().c_str(), sockfd);
            

            //version 2 --- 多线程版本   不能关闭fd了，也不需要了
             pthread_t tid;
             ThreadData *td = new ThreadData(sockfd, this, addr);
             pthread_create(&tid, nullptr, Execute, td);//新线程进行分离

            
        }
        _isrunning = false;
        
    } 

    static void* Execute(void * args)
    {
        pthread_detach(pthread_self());
        ThreadData *td = static_cast<ThreadData*> (args);
        td->_self->_service(td->_sockfd, td->_addr);
        ::close(td->_sockfd);
        delete td;
        return nullptr;
    }

    // void Service(int sockfd, InetAddr addr)
    // {   
    //     //长服务 
    //     while (true)
    //     {
    //         char inbuffer[1024];
    //         ssize_t n = ::read(sockfd, inbuffer, sizeof(inbuffer)-1);
    //         if(n > 0)
    //         {
    //             inbuffer[n] = 0;
    //             LOG(INFO, "get message from client %s, message: %s\n", addr.AddStr().c_str(),inbuffer);
    //             std::string echo_string = "[server echo]";
    //             echo_string += inbuffer;
    //             write(sockfd, echo_string.c_str(), echo_string.size());
    //         }
    //         else if(n == 0)
    //         {
    //             LOG(INFO, "client %s quit\n", addr.AddStr().c_str());
    //             break;
    //         }
    //         else{
    //             LOG(ERROR, "read error: %s\n",addr.AddStr().c_str()); 
    //             break;
    //         }
    //     }
        
    // }


    ~TcpServer()
    {}
private:
    uint16_t _port;
    int _listensockfd;
    bool _isrunning;

    command_service_t _service;
};