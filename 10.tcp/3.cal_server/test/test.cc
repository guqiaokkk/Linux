
#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

// struct request req = {10, 20, '+'};
class Request
{
public:
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

        //Json::FastWriter writer;
        Json::StyledWriter writer;
        std::string s = writer.write(root);
        *out = s;
        return true;
    }

    void Deserialize(const std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool res = reader.parse(in,root);
        _x = root["x"].asInt();
        _y = root["y"].asInt();
        _oper = root["oper"].asInt();
    }

    void Print()
    {
        std::cout << _x << std::endl;
        std::cout << _y << std::endl;
        std::cout << _oper << std::endl;
    }


    ~Request()
    {
    }

private:
    int _x;
    int _y;
    char _oper; //+ - % * /
};

// struct request req = {30, 0};
class Response
{
public:
    Response()
    {
    }

    void Serialize(std::string *out)
    {
    }

    void Deserialize(const std::string &in)
    {
    }

    ~Response()
    {
    }

private:
    int result;
    int code; // 0 :success   1:div zero   2:非法操作
};

int main()
{
    // Request req(111, 222, '+');
    // std::string s;
    // req.Serialize(&s);
    // std::cout << s << std::endl;

    std::string s = "{\"oper\":43, \"x\":111, \"y\":222}";
    Request req;
    req.Deserialize(s);
    req.Print();
    return 0;
}