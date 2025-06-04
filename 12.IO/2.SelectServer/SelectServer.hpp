#pragma once

#include <iostream>
#include <sys/select.h>
#include "Socket.hpp"
#include "InetAddr.hpp"

using namespace socket_ns;

class SelectServer
{
    const static int gnum = sizeof(fd_set) * 8;
    const static int gdefaultfd = -1;

public:
    SelectServer(uint16_t port) : _port(port), _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildListenSocket(_port);
    }

    void InitServer()
    {
        for (int i = 0; i < gnum; i++)
        {
            fd_array[i] = gdefaultfd; // 初始化数组
        }
        fd_array[0] = _listensock->Sockfd(); // 默认直接添加listensock到数组中
    }

    // 处理新连接的
    void Accepter()
    {
        InetAddr addr;
        int sockfd = _listensock->Accepter(&addr); // 一定不会阻塞
        if (sockfd > 0)
        {
            LOG(DEBUG, "get a new link, client info %s:%d\n", addr.Ip().c_str(), addr.Port());
            // 已经获得了一个新的sockfd
            // 接下来不能读取，条件不一定满足
            // 通过select。想办法把新的fd添加给select，由select统一进行监管
            // 将新的fd添加到fd_array中即可
            bool flag = false;
            for (int pos = 1; pos < gnum; pos++)
            {
                if (fd_array[pos] == gdefaultfd)
                {
                    flag = true; // 防止数组满了
                    fd_array[pos] = sockfd;
                    LOG(INFO, "add %d to fd_array success!\n", sockfd);
                    break;
                }
            }
            if (!flag)
            {
                LOG(WARNING, "Server is Full\n");
                ::close(sockfd); // 因为满了，只能把新获取上来的关闭了
            }
        }
    }

    // 处理普通的fd 就绪的
    void HandlerIO(int i)
    {
        char buffer[1024];
        ssize_t n = ::recv(fd_array[i], buffer, sizeof(buffer) - 1, 0); // 这里读取不会阻塞
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "client say# " << buffer << std::endl;
            // std::string echo_str = "[server echo info ] ";
            std::string echo_str = "HTTP/1.0 200 OK\r\n\r\n<html><body><h1> hello bite </h1></body></html>";

            // echo_str += buffer;
            ::send(fd_array[i], echo_str.c_str(), echo_str.size(), 0);
        }
        else if (n == 0)
        {
            LOG(INFO, "client quit...\n");
            // 关闭fd
            ::close(fd_array[i]);
            // select不要再关心这个fd了
            fd_array[i] = gdefaultfd;
        }
        else
        {
            LOG(ERROR, "recv error\n");
            // 关闭fd
            ::close(fd_array[i]);
            // select不要再关心这个fd了
            fd_array[i] = gdefaultfd;
        }
    }

    // 一定会存在大量的fd就绪，可能是普通的sockfd，也可能是listensockfd
    void HandlerEvent(fd_set &rfds)
    {
        //事件派发
        for (int i = 0; i < gnum; i++)
        {
            if (fd_array[i] == gdefaultfd)
                continue;
            // fd一定是合法的
            // 合法的fd不一定就绪，判断fd是否就绪
            if (FD_ISSET(fd_array[i], &rfds)) // 检测是否就绪
            {
                // 读事件就绪
                if (_listensock->Sockfd() == fd_array[i])
                {
                    Accepter(); // 连接事件就绪，等价于读事件就绪
                }
                else
                {
                    HandlerIO(i); // 普通的文件描述符，正常的读写
                }
            }
        }
    }

    void Loop()
    {
        while (true)
        {
            // 1.文件描述符初始化
            fd_set rfds;
            FD_ZERO(&rfds);
            int max_fd = gdefaultfd; // 用于填select的第一个参数

            // 2.合法的fd，添加到rfds中
            for (int i = 0; i < gnum; i++)
            {
                if (fd_array[i] == gdefaultfd)
                    continue;
                FD_SET(fd_array[i], &rfds);

                // 2.1.更新出最大的文件fd的值
                if (max_fd < fd_array[i])
                {
                    max_fd = fd_array[i];
                }
            }

            struct timeval timeout = {3, 0};

            // _listensock->Accepter();//不能，listensock && accept 我们也把他们看作IO类的函数，只关心新连接到来，等价于读时间就绪
            int n = ::select(max_fd + 1, &rfds, nullptr, nullptr, nullptr /*&timeout*/);

            switch (n)
            {
            case 0:
                LOG(DEBUG, "time out,%d.%d\n", timeout.tv_sec, timeout.tv_usec);
                break;
            case -1:
                LOG(ERROR, "select error\n");
                break;
            default:
                LOG(INFO, "haved event ready, n : %d\n", n); // 若事件就绪，但不处理，select会一直通知我，直到我处理了
                HandlerEvent(rfds);
                PrintDebug();
                sleep(1);
                break;
            }
        }
    }
    void PrintDebug()
    {
        std::cout << "fd list : ";
        for (int i = 0; i < gnum; i++)
        {
            if (fd_array[i] == gdefaultfd)
                continue;
            std::cout << fd_array[i] << " ";
        }
        std::cout << "\n";
    }
    ~SelectServer() {}

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;

    // select要正常工作，需要借助一个辅助数组，来保存所有合法fd
    int fd_array[gnum];
};