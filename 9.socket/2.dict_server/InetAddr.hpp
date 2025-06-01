#pragma once

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

class InetAddr
{
private:
    void ToHost(const struct sockaddr_in &addr)
    {
        _port = ntohs(addr.sin_port);
        _ip = inet_ntoa(addr.sin_addr);
    }

public:
    InetAddr(const struct sockaddr_in &addr):_addr(addr)
    {
        ToHost(addr);
    }

    std::string Ip()
    {
        return _ip;
    }

    uint16_t Port()
    {
        return _port;
    }
    
    ~InetAddr()
    {}
private:
    std::string _ip;
    uint16_t _port;
    struct sockaddr_in _addr;
};