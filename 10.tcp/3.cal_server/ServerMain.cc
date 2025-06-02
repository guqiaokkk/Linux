#include "TcpServer.hpp"
#include "Service.hpp"
#include "NetCal.hpp"


//  ./tcpserver 8888
int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " local-Port" << std::endl; 
        exit(0);
    }
    uint16_t port = std::stoi(argv[1]);

    NetCal cal;


    //构建TCP服务器
    IOService service(std::bind(&NetCal::Calculator, &cal, std::placeholders::_1));
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        std::bind(&IOService::IOExcute,&service, std::placeholders::_1,std::placeholders::_2),
         port
        );
    tsvr->Loop();


    //tcpsocket 负责获取新连接，进行处理（不管），处理完关闭
     
    return 0;
}