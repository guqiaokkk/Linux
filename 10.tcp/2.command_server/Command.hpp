#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <set>
#include <cstring>

#include "Log.hpp"
#include "InetAddr.hpp"

class Command
{
public:
    Command()
    {
        _safe_command.insert("ls");
        _safe_command.insert("pwd");
        _safe_command.insert("whoami");
        _safe_command.insert("which");

    }
    ~Command()
    {
    }
    bool SafeCheck(const std::string &cmdstr)
    {
        for(auto &cmd : _safe_command)
        {
            if(strncmp(cmd.c_str(), cmd.c_str(), cmd.size()) == 0)
            {
                return true;
            }
        }
        return false;
    }
    std::string Excute(const std::string &cmdstr)
    {
        if(!SafeCheck(cmdstr))
        {
            return "unsafe";
        }

        std::string result;
        FILE *fp = popen(cmdstr.c_str(),"r");//创建一个子进程。在子进程中，将标准输出（stdout）重定向到管道的写端。执行传入的命令
        if(fp)
        {
            char line[1024];
            while(fgets(line,sizeof(line),fp))//父进程连到管道的读端
            {
                result += line;
            }   
            return result;
        }
        return "execute error";
    }

    void HandlerCommand(int sockfd, InetAddr addr)
    {
        // 长服务
        while (true)
        {
            char commandbuf[1024];
            ssize_t n = ::recv(sockfd, commandbuf, sizeof(commandbuf) - 1,0);
            if (n > 0)
            {
                commandbuf[n] = 0;
                LOG(INFO, "get message from client %s, message: %s\n", addr.AddStr().c_str(), commandbuf);
                std::string echo_string = "[server echo]";
                echo_string += commandbuf;
                std::string result = Excute(commandbuf);
                ::send(sockfd, result.c_str(), result.size(),0);
            }
            else if (n == 0)
            {
                LOG(INFO, "client %s quit\n", addr.AddStr().c_str());
                break;
            }
            else
            {
                LOG(ERROR, "read error: %s\n", addr.AddStr().c_str());
                break;
            }
        }
    }

private:
    std::set<std::string> _safe_command;
};