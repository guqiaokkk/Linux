#include "UdpServer.hpp"
#include "Route.hpp"

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
    Route messageRoute;
    service_t message_route = std::bind(&Route::Forward,\
         &messageRoute, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
 
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(message_route,port);
    usvr->InitServer();
    usvr->Start();
    return 0;

}