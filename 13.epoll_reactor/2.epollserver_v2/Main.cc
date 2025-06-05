#include "Log.hpp"
#include "Reactor.hpp"
#include "Listener.hpp"
#include "HandlerConnection.hpp"
#include "PackageParse.hpp" 


#include <iostream>
#include <memory>

using namespace log_ns;

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << "local-port " << std::endl;
        exit(0);
    }
    uint16_t port = std::stoi(argv[1]);

    EnableScreen();//打开日志

    InetAddr localaddr("0.0.0.0", port);

    //报文解析模块
    PackageParse parse;


    //专门用来处理新连接到来
    Listener listener(port);//连接管理器

    //专门用来处理普通sockfd的模块
    HandlerConnection handlers(std::bind(&PackageParse::Excute, &parse, std::placeholders::_1));//IO处理器

    //主模块，事件派发
    std::unique_ptr<Reactor> R = std::make_unique<Reactor>();//事件派发器
  
    //模块间产生关联

    //先注册方法
    R->SetOnConnect(std::bind(&Listener::Accepter, &listener, std::placeholders::_1));

    R->SetOnNormalHandler(
        std::bind(&HandlerConnection::HandlerRecver, &handlers, std::placeholders::_1),
        std::bind(&HandlerConnection::HandlerSender, &handlers, std::placeholders::_1),
        std::bind(&HandlerConnection::HandlerExcepter, &handlers, std::placeholders::_1)
    );

    //此时一个连接都没，先添加一个新连接
    R->AddConnection(listener.ListenSocked(), EPOLLIN|EPOLLET, localaddr, ListenConnection);


    R->Dispatcher();

    return 0;
}