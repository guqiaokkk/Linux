#include "TcpServer.hpp"
#include "Http.hpp"

HttpResponse Login(HttpRequest &req)
{
    HttpResponse resp;
    std::cout << "外部已经拿到了参数 :" << std::endl;
    req.GetResultBody();
    std::cout << "###################" << std::endl;

    resp.AddCode(200,"OK");
    resp.AddBodyText("<html><h1>result done</h1></html>");

    // 1. pipe
    // 2. dup2
    // 3. fork();
    // 4. exec* -> python, PHP, 甚至是Java！

    return resp;
}


// ./tcpserver 8888
int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " local-Port" << std::endl; 
        exit(0);
    }
    uint16_t port = std::stoi(argv[1]);

    HttpServer hserver;
    hserver.InsertService("/login", Login);//向服务器注册服务
  
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        std::bind(&HttpServer::HandlerHttpRequest,&hserver, std::placeholders::_1),
         port
        );
    tsvr->Loop();


    //tcpsocket 负责获取新连接，进行处理（不管），处理完关闭
     
    return 0;
}


// #include <iostream>
// #include <cstdio>
// #include <string>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <sys/socket.h>

// int main()
// {
//     int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//     if (sockfd < 0) {
//         perror("socket error");
//         return -1;
//     }

//     struct sockaddr_in addr;
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(8888);
//     addr.sin_addr.s_addr = inet_addr("0.0.0.0");
//     socklen_t len = sizeof(struct sockaddr_in);
//     int ret = bind(sockfd, (struct sockaddr*)&addr, len);
//     if (ret < 0) {
//         perror("bind error");
//         return -1;
//     }

//     ret = listen(sockfd, 5);
//     if (ret < 0) {
//         perror("listen error");
//         return -1;
//     }

//     while(1) {
//         int newfd = accept(sockfd, NULL, 0);
//         if (newfd < 0) {
//             perror("accept error");
//             return -1;
//         }

//         char buf[1024] = {0};
//         int ret = recv(newfd, buf, 1024, 0);
//         if (ret < 0) {
//             perror("recv error");
//             close(newfd);
//             continue;
//         }else if (ret == 0) {
//             perror("peer shutdown!");
//             close(newfd);
//             continue;
//         }

//         std::string body = "<html><body><h1>Hello World</h1></body></html>";
//         std::string rsp = "HTTP/1.1 200 OK\r\n";
//         rsp += "Content-Length: " + std::to_string(body.size()) + "\r\n";
//         rsp += "Content-Type: text/html\r\n";
//         rsp += "\r\n";
//         rsp += body;

//         ret = send(newfd, rsp.c_str(), rsp.size(), 0);
//         if (ret < 0) {
//             perror("send error!");
//             close(newfd);
//             continue;
//         }
//         close(newfd);
//     }
//     close(sockfd);
//     return 0;
// }