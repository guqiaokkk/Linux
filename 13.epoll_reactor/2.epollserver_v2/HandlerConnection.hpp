// 处理其他普通的链接

#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

#include "Connection.hpp"
#include "Log.hpp"

using namespace log_ns;
const static int buffersize = 512;

// 不应该让HandlerConnection来处理报文，他只要负责IO就好
class HandlerConnection
{
public:
    HandlerConnection(handler_t process) : _process(process)
    {
    }

    void HandlerRecver(Connection *conn) // conn就是就绪的conn
    {
        errno = 0;
        // 1.直接读
        //  LOG(DEBUG, "client 给我发了消息:%d\n", conn->Sockfd());
        while (true)
        {
            char buffer[buffersize];
            ssize_t n = ::recv(conn->Sockfd(), buffer, sizeof(buffer) - 1, 0);
            if (n > 0)
            {
                buffer[n] = 0;                // 数据块
                conn->AppendInbuffer(buffer); // 每个fd也就是每个connection，有自己的接收和输出缓冲区
            }
            else
            {
                if (errno == EWOULDBLOCK)
                {
                    break;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    conn->_handler_excepter(conn); // 统一执行异常处理
                    return;
                }
            }
        }
        // 2.交给业务处理
        //  走到这，一定是读取完毕了,我们应该处理数据了
        std::cout << "%d Inbuffer内容: " << conn->Inbuffer() << std::endl;



        //要改为线程池，这里可以让他不做处理，让他把报文解析，处理的方法，作为任务交给线程池
        _process(conn); // 内容分析
    }

    void HandlerSender(Connection *conn)
    {
        errno = 0;
        // 1.直接写
        while (true)
        {
            ssize_t n = ::send(conn->Sockfd(), conn->Outbuffer().c_str(), conn->Outbuffer().size(), 0);
            if (n > 0)
            {
                conn->DiscardOutbuffer(n);
                if (conn->Outbuffer().empty())
                    break;
            }
            else if (n == 0)
            {
                break;
            }
            else
            {
                if (errno == EWOULDBLOCK) // 写满了
                {
                    // 发送条件不满足
                    break;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    conn->_handler_excepter(conn);
                    return;
                }
            }
        }
        // 2.只能是发送条件不满足
        //缓冲区还有数据
        if(!conn->Outbuffer().empty())
        {
            //开启对写事件关心
            conn->_R->EnableConnectionReadWrite(conn->Sockfd(), true, true);
        }
        else
        {
            conn->_R->EnableConnectionReadWrite(conn->Sockfd(), true, false);
        }
    }

    void HandlerExcepter(Connection *conn)
    {
        //整个代码所有的逻辑异常处理，全部都在这里
        //删除连接
        conn->_R->DelConnection(conn->Sockfd()); 
    }

private:
    handler_t _process;
};