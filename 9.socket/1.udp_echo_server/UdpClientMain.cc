#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



//客户端一定要先知道服务器的IP地址和端口号
// ./udp_client Server-ip  Server-Port

int main(int argc,char *argv[])
{
    if(argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " Server-ip Server-Port" << std::endl;
        exit(0);
    }

    std::string serverip = argv[1];            //ip
    uint16_t serverport = std::stoi(argv[2]);  //port

    int sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        std::cerr << "create sockfd error" << std::endl;
        exit(1);
    }

    //client的端口号，一般不让用户自己设定，而是让client OS随机选择 
    //client 需要bind他自己的IP和端口号（因为网络通信本质是进程间通信），但是不需要 显示 bind他自己的IP和端口号
    //client在首次向服务器发送数据时，OS会自动给client bind他自己的IP和端口

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    server.sin_addr.s_addr = inet_addr(serverip.c_str());

    while (1)
    {
        std::string line;
        std::cout << "Please Enter# ";
        std::getline(std::cin, line);
        
        //要发消息得知道发给谁，通过server可知
        int n = sendto(sockfd, line.c_str(), line.size(), 0, (struct sockaddr*)&server, sizeof(server));
    
        if(n > 0)
        {
            struct sockaddr_in temp;
            socklen_t len = sizeof(temp);
            char buffer[1024];
            int m = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&temp, &len) ;
            if(m > 0)
            {
                buffer[m] = 0;
                std::cout << buffer << std::endl;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    

    ::close(sockfd);
    return 0;
}