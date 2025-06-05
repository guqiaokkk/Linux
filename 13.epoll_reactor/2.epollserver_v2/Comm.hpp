#pragma once

#include <fcntl.h>
#include <iostream>
#include <unistd.h>

enum
{
    SOCKET_ERROR = 1,
    BIND_ERROR,
    LISTEN_ERROR,
    EPOLL_CREATE_ERROR
};

void SetNonBlock(int fd) // 设为非阻塞
{
    int fl = ::fcntl(fd, F_GETFL);
    if (fl < 0)
    {
        std::cout << "fcntl error" << std::endl;
        return;
    }
    ::fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}