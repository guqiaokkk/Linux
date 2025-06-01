#include "UdpServer.hpp"
#include "Dict.hpp"

#include <memory>


// ./udp_server local-Port
// ./udp_server 8888
int main(int argc,char *argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " local-Port" << std::endl;
        exit(0);
    } //本主机，localhost
    uint16_t port = std::stoi(argv[1]);  //port

    EnableScreen();
    
    Dict dict("./dict1.txt");
    func_t translate = std::bind(&Dict::Translate, &dict, std::placeholders::_1);

    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(translate,port);
    usvr->InitServer();
    usvr->Start();
    return 0;

}