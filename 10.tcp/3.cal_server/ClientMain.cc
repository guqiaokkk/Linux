#include <iostream>
#include <ctime>
#include <unistd.h>

#include "Socket.hpp"
#include "Protocol.hpp"

using namespace socket_ns;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << "server-IP server PORT" << std::endl;
        exit(0);
    }
    std::string severip = argv[1];
    uint16_t severport = std::stoi(argv[2]);

    SockSPtr sock = std::make_shared<TcpSocket>();
    if (!sock->BuildClientSocket(severip, severport))
    {
        std::cerr << "connect error" << std::endl;
        exit(1);
    }

    srand(time(nullptr) ^ getpid());
    const std::string opers = "+-*/%^!&";

    std::string packagestreamqueue;
   // int cnt = 3;
    while (true)
    {
        // 构建数据
        int x = rand() % 10;
        usleep(1000);
        int y = rand() % 10;
        usleep(x * y * 100);
        char oper = opers[y % opers.size()];

        // 构建请求
        auto req = Factory::BuildRequestDefault();
        req->SetValue(x, y, oper);

        // 1.序列化
        std::string reqstr;
        req->Serialize(&reqstr);

        // 2.添加长度报头字段
        reqstr = Encode(reqstr);

        std::cout << "####################################" << std::endl;

        std::cout << "request string: \n" << reqstr << std::endl;

        // 3.发送数据
        sock->Send(reqstr);

        // 4.读取应答,response
        while (true)
        {
            // 负责读取
            ssize_t n = sock->Recv(&packagestreamqueue);
            if (n <= 0)
            {
                break;
            }
            // 不能保证读到的是一个完整的报文
            // 5.报文解析，提取有效报头和有效载荷
            std::string package = Decode(packagestreamqueue);
            if (package.empty())
                continue;

            std::cout << "package: \n" << package << std::endl;

            //6.反序列化
            auto resp = Factory::BuildResponseDefault();
            resp->Deserialize(package);

            //7.打印结果
            resp->PrintResult();

            break;
        }
        sleep(1);
    }

    sock->Close();

    return 0;
}
