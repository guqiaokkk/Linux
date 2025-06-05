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
#include <memory>

#include "Log.hpp"
#include "InetAddr.hpp"
#include "Comm.hpp"



namespace socket_ns
{
    using namespace log_ns;
 
    class Socket;
    using SockSPtr = std::shared_ptr<Socket>;

    


    const static int gblcklog = 8;

    // 模板方法模式
    class Socket
    {
    public:
        virtual void CreateSocketOrDie() = 0;
        virtual void CreateBindOrDie(uint16_t port) = 0;
        virtual void CreateListenOrDie(int backlog = gblcklog) = 0;
        virtual int Accepter(InetAddr *cliaddr, int *code) = 0;
        virtual bool Conntecor(const std::string &peerip, uint16_t peerport) = 0;
        virtual int Sockfd() = 0;
        virtual void Close() = 0;

        virtual ssize_t Recv(std::string *out) = 0;
        virtual ssize_t Send(const std::string &in) = 0;



        virtual void ReUseAddr() = 0;
    public:
        void BuildListenSocket(uint16_t port)
        {
            CreateSocketOrDie();

            ReUseAddr();
            CreateBindOrDie(port);
            CreateListenOrDie();
        }

        bool BuildClientSocket(const std::string &peerip, uint16_t peerport)
        {
            CreateSocketOrDie();
            return Conntecor(peerip, peerport);
        }
        // void BUildUdpSocket()
        // {}
    };

  


    class TcpSocket : public Socket
    {
    public:
        TcpSocket(int sockfd) : _sockfd(sockfd)
        {
        }
        TcpSocket()
        {
        }
        ~TcpSocket()
        {
        }

        void CreateSocketOrDie() override
        {
            // 1.创建socket
            _sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                LOG(FATAL, "socket create error\n");
                exit(SOCKET_ERROR);
            }
            
            SetNonBlock(_sockfd);//设为非阻塞
            LOG(INFO, "socket create success, sockfd: %d\n", _sockfd);
        }

        void CreateBindOrDie(uint16_t port) override
        {
            struct sockaddr_in local;
            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(port);
            local.sin_addr.s_addr = INADDR_ANY;

            // bind sockfd 和 Socket addr
            if (::bind(_sockfd, (struct sockaddr *)&local, sizeof(local)) < 0)
            {
                LOG(FATAL, "bind error\n");
                exit(BIND_ERROR);
            }
           // LOG(INFO, "bind success, sockfd: %d\n", _sockfd);
        }

        void CreateListenOrDie(int backlog = gblcklog) override
        {
            if (::listen(_sockfd, gblcklog) < 0)
            {
                LOG(FATAL, "listen error\n");
                exit(LISTEN_ERROR);
            }
            //LOG(INFO, "listen success");
        }

        int Accepter(InetAddr *cliaddr, int *code) override
        {
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            // 获取新连接
            int sockfd = ::accept(_sockfd, (struct sockaddr *)&client, &len);
            *code = errno;//带出错误码
            if (sockfd < 0)
            {
                return -1;
            }

            SetNonBlock(_sockfd);//设为非阻塞

            *cliaddr = InetAddr(client);
            LOG(INFO, "get a new link, client info : %s, sockfd is : %d\n", cliaddr->AddStr().c_str(), sockfd);

            //return std::make_shared<TcpSocket>(sockfd);
            return sockfd;
        }

        bool Conntecor(const std::string &peerip, uint16_t peerport) override
        {
            struct sockaddr_in server;
            memset(&server, 0, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_port = htons(peerport);
            ::inet_pton(AF_INET, peerip.c_str(), &server.sin_addr);

            int n = ::connect(_sockfd, (struct sockaddr *)&server, sizeof(server));
            if (n < 0)
            {
                return false;
            }
            return true;
        }

        int Sockfd()
        {
            return _sockfd;
        }

        void Close()
        {
            if (_sockfd > 0)
            {
                ::close(_sockfd);
            }
        }

        ssize_t Recv(std::string *out) override
        {
            char inbuffer[4096];
            ssize_t n = ::recv(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0);
            if (n > 0)
            {
                inbuffer[n] = 0;
                *out += inbuffer;
            }
            return n;
        }

        ssize_t Send(const std::string &in) override
        {
            return ::send(_sockfd, in.c_str(), in.size(), 0);
        }


        void ReUseAddr() override
        {
            int opt = 1;
            ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        }//允许在端口处于 TIME_WAIT 状态时重新绑定该端口。
        //在频繁重启服务时，可以避免因端口被占用而导致的绑定失败问题。


    private:
        int _sockfd; // 可以是listensockfd，可以是普通sockfd....
    };

    // Socket *sock = new TcpSocket();
    // sock->BuildListenSocket();

    // class UdpSocket : public Socket
    // {};

}