#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Thread.hpp"

using namespace ThreadMoudle;

int InitClient()
{
    int sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "create sockfd error" << std::endl;
        exit(1);
    }
    return sockfd;
}

// 客户端一定要先知道服务器的IP地址和端口号
//  ./udp_client Server-ip  Server-Port

void RecvMessage(int sockfd, const std::string &name)
{
    while (true)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        char buffer[1024];
        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cerr << buffer << std::endl;
        }
        else
        {
            break;
        }
    }
}

void SendMessage(int sockfd, std::string serverip, uint16_t serverport, const std::string &name)
{

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    server.sin_addr.s_addr = inet_addr(serverip.c_str());

    std::string cli_profix = name + "# ";
    while (true)
    {
        std::string line;
        std::cout << cli_profix;
        std::getline(std::cin, line);

        int n = sendto(sockfd, line.c_str(), line.size(), 0, (struct sockaddr *)&server, sizeof(server)); 
        // 要发消息得知道发给谁，通过server可知
        if (n <= 0)
        {
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " Server-ip Server-Port" << std::endl;
        exit(0);
    }

    std::string serverip = argv[1];           // ip
    uint16_t serverport = std::stoi(argv[2]); // port

    int sockfd = InitClient();

    Thread recver("recver-thread", std::bind(&RecvMessage, sockfd, std::placeholders::_1));
    Thread sender("sender-thread", std::bind(&SendMessage, sockfd, serverip, serverport, std::placeholders::_1));

    recver.Start();
    sender.Start();

    recver.Join();
    sender.Join();


   ::close(sockfd);
    return 0;
}