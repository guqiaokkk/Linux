#include "TcpServer.hpp"
#include "Command.hpp"

#include <memory>

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " local-Port" << std::endl;
        exit(0);
    }
    uint16_t port = std::stoi(argv[1]);

    Command cmdservice;
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        std::bind(&Command::HandlerCommand,
        &cmdservice,std::placeholders::_1,
        std::placeholders::_2),
        port);
    
    tsvr->InitServer();
    tsvr->Loop();



    return 0;
}