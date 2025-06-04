#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <iostream>


void SetNonBlock(int fd)
{
    int fl = ::fcntl(fd, F_GETFL);
    if(fl < 0)
    {
        std::cout << "fcntl error" << std::endl;
        return;
    }
    ::fcntl(fd, F_SETFL, fl | O_NONBLOCK);//设置为非阻塞
}