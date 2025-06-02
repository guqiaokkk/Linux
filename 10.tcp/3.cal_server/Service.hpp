#pragma once

#include <iostream>
#include <functional>

#include "InetAddr.hpp"
#include "Socket.hpp"
#include "Log.hpp"
#include "Protocol.hpp"

using namespace socket_ns;
using namespace log_ns;

using process_t = std::function<std::shared_ptr<Response>(std::shared_ptr<Request>)>;//参数为请求，返回值为响应

class IOService
{
public:
    IOService(process_t process):_process(process)
    {
    }

    void IOExcute(SockSPtr sock, InetAddr &addr)
    {
        std::string packagestreamqueue;
        while (true)
        {
            //1.负责读取
            ssize_t n = sock->Recv(&packagestreamqueue);
            if(n <= 0)
            {
                LOG(INFO, "client %s quit or recv error\n", addr.AddStr().c_str());
                break;
            }
            std::cout << "-------------------------------------------" << std::endl;
            std::cout << "packagestreamqueue:\n" << packagestreamqueue << std::endl;

            //不能保证读到的是一个完整的报文
            //2.报文解析，提取有效报头和有效载荷
            std::string package = Decode(packagestreamqueue);
            if(package.empty()) continue;

            //能保证读到的是一个完整的报文
            auto req = Factory::BuildRequestDefault();

            std::cout << "package:\n" << package << std::endl;

            //3.反序列化
            req->Deserialize(package);

            //4.业务处理
            auto resp = _process(req);//通过请求，得到应答
        
            //5.序列化应答
            std::string respjson;
            resp->Serialize(&respjson);

            std::cout << "respjson:\n" << respjson << std::endl;

            //6.添加len长度报头
            respjson =  Encode(respjson);
            std::cout << "respjson add header done:\n" << respjson << std::endl;
            //构建了一个携带报头的应答json串

            //7.发送回去
            sock->Send(respjson);
        }
    }

    ~IOService()
    {
    }
private:
    process_t _process;
};