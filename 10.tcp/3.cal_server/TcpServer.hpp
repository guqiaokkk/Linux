#pragma once

#include <functional>

#include "Socket.hpp"
#include "Log.hpp"
#include "InetAddr.hpp"



 
using namespace socket_ns;

static const int gport = 8888;

using service_io_t = std::function<void(SockSPtr, InetAddr&)>;


class TcpServer
{
public:
    TcpServer(service_io_t service, int port = gport)
     :_port(port),
     _listensock(std::make_shared<TcpSocket>()),
     _isrunning(false),
     _service(service)

    {
        _listensock->BuildListenSocket(_port);
    }
 
    class ThreadData
    {
    public: 
        SockSPtr _sockfd;
        TcpServer *_self;
        InetAddr _addr;
    public:
        ThreadData(SockSPtr sockfd, TcpServer* self, const InetAddr &addr):_sockfd(sockfd),_self(self),_addr(addr)
        {}
    };

    void Loop()
    {
        _isrunning = true;
        while (_isrunning)
        {
            InetAddr client;
            SockSPtr newsock = _listensock->Accepter(&client);
            if(newsock == nullptr)
                continue;
            LOG(INFO, "get a new link, client info: %s, sockfd is : %d\n",client.AddStr().c_str(), newsock->Sockfd());
            

            //version 2 --- 多线程版本   不能关闭fd了，也不需要了
             pthread_t tid;
             ThreadData *td = new ThreadData(newsock, this, client);
             pthread_create(&tid, nullptr, Execute, td);//新线程进行分离

            
        }
        _isrunning = false;
        
    } 

    static void* Execute(void * args)
    {
        pthread_detach(pthread_self());
        ThreadData *td = static_cast<ThreadData*> (args);
        td->_self->_service(td->_sockfd, td->_addr);
        td->_sockfd->Close();
        delete td;
        return nullptr;
    }

    

    ~TcpServer()
    {}
private:
    uint16_t _port;
    SockSPtr _listensock;
    bool _isrunning;

    service_io_t _service;
};