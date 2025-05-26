#pragma once
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <cerrno>
#include <string>
#include <unistd.h>
#include<fcntl.h>

const std::string comm_path = "./myfifo";

#define Creater 1
#define User 2
#define DefaultFd -1
#define Read O_RDONLY
#define Write O_WRONLY
#define Basesize 4096

//命名管道 


class NamePiped
{
private:
bool OpenNamePipe(int mode)
{
    _fd = open(_fifo_path.c_str(),mode);
    if(_fd < 0)return false;
    return true;
}
public:
    NamePiped(const std::string &path, int who) : _fifo_path(path), _id(who),_fd(DefaultFd)
    {
        if (_id == Creater)
        {
            int res = mkfifo(path.c_str(), 0666);
            if (res != 0)
            {
                perror("mkfifo");
            }
        }
    }

    bool OpenForRead()
    {
        return OpenNamePipe(Read);
    }
    bool OpenForWrite()
    {
        return OpenNamePipe(Write);
    }

    int ReadNamePipe(std::string *out)
    {
        char buffer[Basesize];
        int n = read(_fd,buffer,sizeof(buffer));
        if(n > 0)
        {
            buffer[n] = 0;
            *out = buffer;
        }
        return n;
    }

    int WriteNamePipe(const std::string &in)
    {
        return write(_fd,in.c_str(),in.size());
    }


    ~NamePiped()
    {
        if (_id == Creater)
        {
            int res = unlink(_fifo_path.c_str());
            if (res != 0)
            {
                perror("unlink");
            }
        }
        if(_fd != DefaultFd)close(_fd);
    }

private:
    const std::string _fifo_path;
    int _id;
    int _fd;
};
