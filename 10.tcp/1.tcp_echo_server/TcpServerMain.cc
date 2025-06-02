#include "TcpServer.hpp"

#include <memory>

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " local-Port" << std::endl;
        exit(0);
    }
    uint16_t port = std::stoi(argv[1]);

    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(port);
    
    tsvr->InitServer();
    tsvr->Loop();



    return 0;
}