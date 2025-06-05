#pragma once

// 此类未来统一进行listensock的管理工作，获取新连接

#include <iostream>
#include <memory>

#include "Socket.hpp"
#include "Connection.hpp"

using namespace socket_ns;

class Listener
{
public:
    Listener(uint16_t port) : _port(port), _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildListenSocket(_port);
    }

    int ListenSocked()
    {
        return _listensock->Sockfd();
    }

    void Accepter(Connection *conn)
    {
        while (true) // 因为此时是ET模式，所以要不断获取此次的新连接
        {
            errno = 0;
            InetAddr addr;
            int code = 0;
            int sockfd = _listensock->Accepter(&addr, &code);//所以这里得要非阻塞.直接在Socket.hpp中 SetNonBlock(_sockfd);//设为非阻塞

            if (sockfd > 0)
            {
                LOG(INFO, "获取链接成功,客户端: %s:%d, sockfd : %d\n", addr.Ip().c_str(), addr.Port(),sockfd);
                //链接后该干什么？
                conn ->_R->AddConnection(sockfd, EPOLLIN | EPOLLET, addr, NormalConnection);//处理普通套接字,最后跳到HandlerConnection.hpp
            }
            else
            {
                if(code == EWOULDBLOCK)//链接读完了
                {
                    LOG(INFO, "底层链接全部获取完毕\n");
                    break;
                }
                else if(code == EINTR)//信号中断了
                {
                    continue;
                }
                else
                {
                    LOG(ERROR, "获取链接失败!\n");
                    break;
                }
            }
        }
    }

    ~Listener()
    {
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
};