//报文解析的模块

#pragma once

#include <iostream>
#include <functional>

#include "InetAddr.hpp"
#include "Log.hpp"
#include "Protocol.hpp"
#include "Connection.hpp"
#include "NetCal.hpp"

using namespace log_ns;


class PackageParse
{
public:
    
    void Excute(Connection *conn)
    {
        while (true)
        {

            //不能保证读到的是一个完整的报文
            //2.报文解析，提取有效报头和有效载荷
            std::string package = Decode(conn->Inbuffer());
            if(package.empty())
                 break;

            //能保证读到的是一个完整的报文
            auto req = Factory::BuildRequestDefault();

            std::cout << "package:\n" << package << std::endl;

            //3.反序列化
            req->Deserialize(package);

            //4.业务处理
            //auto resp = _process(req);//通过请求，得到应答
            auto resp = cal.Calculator(req);   

            //5.序列化应答
            std::string respjson;
            resp->Serialize(&respjson);

            std::cout << "respjson:\n" << respjson << std::endl;

            //6.添加len长度报头
            respjson =  Encode(respjson);
            std::cout << "respjson add header done:\n" << respjson << std::endl;
            //构建了一个携带报头的应答json串

            //7.发回
            conn->AppendOutbuffer(respjson);
        }

        //此时已经至少处理了一个请求,同时至少会有一个应答
       // if(!conn->Outbuffer().empty())conn->_handler_sender(conn);//法一：直接发送数据

        //法二：我不发，我只要进行激活对写事件的关心
        if(!conn->Outbuffer().empty())
            conn->_R->EnableConnectionReadWrite(conn->Sockfd(), true, true);
    }
private:
    NetCal cal;
};