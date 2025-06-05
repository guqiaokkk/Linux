#include "EpollServer.hpp"

#include <memory>

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << "local-port " << std::endl;
        exit(0);
    }
    uint16_t port = std::stoi(argv[1]);

    EnableScreen();//打开日志
    std::unique_ptr<EpollServer> svr = std::make_unique<EpollServer>(port);
    svr->InitServer();
    svr->Loop();


    return 0;
}