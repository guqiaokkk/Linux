#pragma once

#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>


//设计一下协议的报头和报文的完整格式
// "len"\r\n"{json}"\r\n ---完整的报文，len有效载荷的长度
// 1\r\n : 区分len和json串
// 2\r\n : 便于打印


static const std::string sep = "\r\n";
//添加报头
std::string Encode(const std::string &jsonstr)
{
    int len = jsonstr.size();
    std::string lenstr = std::to_string(len);
    return lenstr + sep + jsonstr + sep;
}

//不能带const
// "len"\r\n"{json}"\r\n
std::string Decode(std::string &packagestream)
{
    //分析有没有完整报文
    auto pos = packagestream.find(sep);
    if(pos == std::string::npos) return std::string();
    std::string lenstr = packagestream.substr(0,pos);
    int len = std::stoi(lenstr);
    //计算一个报文的总长度 
    int total = lenstr.size() + len + 2 * sep.size();
    if(packagestream.size() < total)return std::string();

    //提取
    std::string jsonstr = packagestream.substr(pos + sep.size(), len);
    packagestream.erase(0,total);
    return jsonstr;
}




//struct request req = {10, 20, '+'};
class Request
{
public:
   
// struct request req = {10, 20, '+'};
    Request()
    {}
    Request(int x, int y, char oper) : _x(x), _y(y), _oper(oper)
    {
    }

    bool Serialize(std::string *out)
    {
        // 自己做or用现成的库，xml,json(jsoncpp),protobuf
        Json::Value root;
        root["x"] = _x;
        root["y"] = _y;
        root["oper"] = _oper;

        Json::FastWriter writer;
        //Json::StyledWriter writer;
        std::string s = writer.write(root);
        *out = s;
        return true;
    }

    bool Deserialize(const std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool res = reader.parse(in,root);
        if(!res) return false;
        _x = root["x"].asInt();
        _y = root["y"].asInt();
        _oper = root["oper"].asInt();

        return true;
    }

    void Print()
    {
        std::cout << _x << std::endl;
        std::cout << _y << std::endl;
        std::cout << _oper << std::endl;
    }

    int X()
    {
        return _x;
    }

    int Y()
    {
        return _y;
    }

    char Oper()
    {
        return _oper;
    }

    void SetValue(int x, int y, char oper)
    {
        _x = x;
        _y = y;
        _oper = oper;
    }


    ~Request()
    {
    }

private:
    int _x;
    int _y;
    char _oper; //+ - % * /
};



//struct request req = {30, 0};
class Response
{
public:
    Response():_result(0),_code(0),_desc("success")
    {}

    bool Serialize(std::string *out)
    {
        Json::Value root;
        root["result"] = _result;
        root["code"] = _code;
        root["desc"] = _desc;

        Json::FastWriter writer;
        //Json::StyledWriter writer;
        std::string s = writer.write(root);
        *out = s;
        return true;
    }

    bool Deserialize(const std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool res = reader.parse(in,root);
        if(!res)
            return false;
        _result = root["result"].asInt();
        _code = root["code"].asInt();
        _desc = root["desc"].asString();

        return true;
    }

    void PrintResult()
    {
        std::cout << "result:" << _result << "," << "code:" << _code << "," << "desc:" << _desc << std::endl;
    }


    ~Response()
    {}


public:
    int _result;
    int _code; // 0 :success   1:div zero   2:非法操作
    std::string _desc;
};

class Factory
{
public:
    static std::shared_ptr<Request> BuildRequestDefault()
    {
        return std::make_shared<Request>();
    }
    static std::shared_ptr<Response> BuildResponseDefault()
    {
        return std::make_shared<Response>();
    }
    
};