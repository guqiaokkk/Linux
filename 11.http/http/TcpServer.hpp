#pragma once

#include <functional>

#include "Socket.hpp"
#include "Log.hpp"
#include "InetAddr.hpp"



 
using namespace socket_ns;

static const int gport = 8888;

using service_t = std::function<std::string(std::string &requeststr)>;


class TcpServer
{
public:
    TcpServer(service_t service, int port = gport)
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
            std::cout << "acceptor" << std::endl;
            SockSPtr newsock = _listensock->Accepter(&client);
            if(newsock == nullptr)
                continue;

            std::cout << "acceptor" << std::endl;
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

        std::string requeststr;
        ssize_t n = td->_sockfd->Recv(&requeststr); // 不做请求的分离了。我们认为读到的是一个完整的请求
        if(n > 0)
        {
            std::string responsestr = td->_self->_service(requeststr);

           // std::cout << "1111111111111111111111111" << std::endl;
            std::cout << responsestr << std::endl;
            td->_sockfd->Send(responsestr);
        }

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

    service_t _service;
};