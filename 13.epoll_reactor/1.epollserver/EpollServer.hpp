#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <sys/epoll.h>

#include "Log.hpp"
#include "Socket.hpp"

using namespace socket_ns;

class EpollServer
{
    const static int size = 128; // 无意义，只要大于0，为了epoll_create
    const static int num = 128;

public:
    EpollServer(uint16_t port) : _port(port), _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildListenSocket(port);
        _epfd = ::epoll_create(size);
        if (_epfd < 0)
        {
            LOG(FATAL, "epoll create error!\n");
            exit(1);
        }
        LOG(INFO, "epoll create success, enfd: %d\n", _epfd); // listen 3, epfd 4
    }

    void InitServer()
    {
        // 新连接到来，我们认为是读事件就绪
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = _listensock->Sockfd(); // 为了在事件就绪的时候，得到是哪一个fd就绪了

        // 必须先把listen sock添加到epoll 中
        int n = ::epoll_ctl(_epfd, EPOLL_CTL_ADD, _listensock->Sockfd(), &ev);
        if (n < 0)
        {
            LOG(FATAL, "epoll_ctl error!\n");
            exit(1);
        }
        LOG(INFO, "epoll_ctl success, add new sockfd success : %d\n", _listensock->Sockfd());
    }

    std::string EventsTOString(uint32_t events)
    {
        std::string eventstr;
        if (events & EPOLLIN)
            eventstr = "EPOLLIN";
        if (events & EPOLLOUT)
            eventstr += " | EPOLLOUT";
        return eventstr;
    }

    void Accepter() // 链接获取模块
    {
        InetAddr addr;
        int sockfd = _listensock->Accepter(&addr); // 肯定不会被阻塞
        if (sockfd < 0)
        {
            LOG(ERROR, "获取链接失败\n");
            return;
        }
        LOG(INFO, "得到一个新的链接: %d,client : %s:%d\n", sockfd, addr.Ip().c_str(), addr.Port());
        // 得到了一个新的sockfd
        // 等底层有数据，read/recv才不会被阻塞
        // epoll最清楚底层是否有数据，将新的sockfd添加到epoll中
        struct epoll_event ev;
        ev.data.fd = sockfd;
        ev.events = EPOLLIN;
        ::epoll_ctl(_epfd, EPOLL_CTL_ADD, sockfd, &ev);
        LOG(INFO, "epoll_ctl success, add new sockfd success : %d\n", sockfd);
    }

    void HandlerIO(int fd)
    {
        char buffer[4096];
        //不能保证buffer是一个完整的，或者有多个请求
        //一个fd，要有一个自己的缓冲区
        //引入协议
        int n = ::recv(fd, buffer, sizeof(buffer) - 1, 0); // 不会阻塞
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << buffer;
            std::string response = "HTTP/1.0 200 OK\r\n";
            std::string content = "<html><body><h1> hello 111, hello world </h1></body></html>";
            response += "Content-Type: text/html\r\n";
            response += "Content-Length:" + std::to_string(content.size()) + "\r\n";
            response += "\r\n";
            response += content;

            ::send(fd, response.c_str(), response.size(), 0);
        }
        else if (n == 0)
        {
            LOG(INFO, "client quit, close fd : %d\n", fd);
            // 1.从epoll中移除. 从epoll中移除fd，这个fd必须是健康合法的，否则移除会出错
            //  所以先移除再关闭
            ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
            // 2.关闭fd
            ::close(fd);
        }
        else // ERROR
        {
            LOG(ERROR, "recv error, close fd : %d\n", fd);
            ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
            ::close(fd);
        }
    }

    void HandlerEvent(int n)
    {
        for (int i = 0; i < n; i++)
        {
            int fd = revs[i].data.fd;
            uint32_t revents = revs[i].events;

            LOG(INFO, "%d 上面有事件就绪了，具体事件是: %s\n", fd, EventsTOString(revents).c_str());

            if (revents & EPOLLIN)
            {
                //_listensock 读事件就绪
                // 新连接到来
                if (fd == _listensock->Sockfd())
                {
                    Accepter();
                }
                else
                {
                    HandlerIO(fd);
                }
            }
        }
    }

    void Loop()
    {
        int timeout = -1;

        while (true)
        {
            int n = ::epoll_wait(_epfd, revs, num, timeout);
            switch (n)
            {
            case 0:
                LOG(INFO, "epoll time out...\n");
                break;
            case -1:
                LOG(ERROR, "epoll error\n");
                break;
            default:
                LOG(INFO, "haved event happend!, n : %d\n", n);
                HandlerEvent(n);

                break;
            }
        }
    }

    ~EpollServer()
    {
        if (_epfd >= 0)
            ::close(_epfd);
        _listensock->Close();
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
    int _epfd; // epoll  fd
    struct epoll_event revs[num];
};