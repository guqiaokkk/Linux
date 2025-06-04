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
        //_ip = inet_ntoa(addr.sin_addr);
        char ip_buf[32];
        ::inet_ntop(AF_INET, &addr.sin_addr, ip_buf, sizeof(ip_buf));
        _ip = ip_buf;
    }

public:
    InetAddr(const struct sockaddr_in &addr):_addr(addr)
    {
        ToHost(addr);
    }
    InetAddr()
    {}

    bool operator == (const InetAddr &addr)
    {
        return (this->_ip == addr._ip && this->_port == addr._port);
    }

    std::string Ip()
    {
        return _ip;
    }

    uint16_t Port()
    {
        return _port;
    }

    struct sockaddr_in Addr()
    {
        return _addr;
    }

    std::string AddStr()
    {
        return _ip + ":" + std::to_string(_port);
    }
    
    ~InetAddr()
    {}
private:
    std::string _ip;
    uint16_t _port;
    struct sockaddr_in _addr;
};