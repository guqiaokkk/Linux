#include <iostream>
#include <cstring>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//  ./tcpclient server-ip server-port

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " Server-ip Server-Port" << std::endl;
        exit(0);
    }

    std::string serverip = argv[1];           // ip
    uint16_t serverport = std::stoi(argv[2]); // port

    //1.创建socket
    int sockfd = ::socket(AF_INET,SOCK_STREAM, 0);
    if(sockfd < 0) 
    {
        std::cerr << "create socket error " << std::endl;
        exit(1);
    }

    //2.不需要显示的bind，但一定要有自己的的IP和port，会隐式的bind，OS会自动bind，用自己的IP和随机的端口
    //当connection成功了，会进行自动bind
    
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    //server.sin_addr   
    ::inet_pton(AF_INET, serverip.c_str(), &server.sin_addr);

    int n = ::connect(sockfd, (struct sockaddr*)&server, sizeof(server));
    if(n < 0)
    {
        std::cerr << "connect socket error " << std::endl;
        exit(2);
    }

    while (true)
    {
        std::string message;
        std::cout << "Enter#";
        std::getline(std::cin, message);

        write(sockfd, message.c_str(), message.size());

        char echo_buffer[1024];
        n = read(sockfd, echo_buffer, sizeof(echo_buffer));
        if(n > 0)
        {
            echo_buffer[n] = 0;
            std::cout << echo_buffer << std::endl;
        }
        else {
            break;
        }
    }
    

    ::close(sockfd);
    return 0;
}