#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <functional>

const static std::string base_sep = "\r\n";
const static std::string line_sep = ": ";
const static std::string prefixpath = "wwwroot";  // web根目录
const static std::string homepage = "index.html"; // 页面
const static std::string httpversion = "HTTP/1.0";
const static std::string spacesep = " "; 
const static std::string suffixsep = ".";
const static std::string html_404 = "404.html";
const static std::string arg_sep = "?";


// 浏览器做对请求的序列化和应答的反序列化，所以我们这里只要实现请求的反序列化，和应答的序列化
// 客户端向服务器发起请求，服务器应答
class HttpRequest
{
private:
    // \r\n
    // \r\n data
    std::string Getline(std::string &reqstr)
    {
        auto pos = reqstr.find(base_sep);
        if (pos == std::string::npos)
            return std::string();
        std::string line = reqstr.substr(0, pos);
        reqstr.erase(0, line.size() + base_sep.size());

        return line.empty() ? base_sep : line; // 如果为空，说明报头部分读完了，一上来就读到\r\n了
    }

    void ParseReqLine()
    {
        std::stringstream ss(_req_line);   // 把字符串转换为字符串流
        ss >> _method >> _url >> _version; // 默认以空格作为分隔符，将ss里的数据写入到>>后的变量里

        //    /login?user = xxxx&password =2131
        if(strcasecmp(_method.c_str(), "GET") == 0)
        {
            auto pos =_url.find(arg_sep);
            if(pos != std::string::npos)
            {
                _body_text = _url.substr(pos + arg_sep.size());
                _url.resize(pos);
            }
        }


        _path += _url;

        if (_path[_path.size() - 1] == '/')
        {
            _path += homepage;
        }

        // wwwroot/index.html
        // wwwroot/image/1.png
        auto pos = _path.rfind(suffixsep); // 找后缀，从后往前找
        if (pos != std::string::npos)
        {
            _suffix = _path.substr(pos);
        }
        else
        {
            _suffix = ".default";
        }
    }

    void ParseReqHeader()
    {
        for (auto &header : _req_headers)
        {
            auto pos = header.find(line_sep);
            if (pos == std::string::npos)
                continue;
            std::string k = header.substr(0, pos);
            std::string v = header.substr(pos + line_sep.size());
            if (k.empty() || v.empty())
                continue;

            _headers_kv.insert(std::make_pair(k, v));
        }
    }

   
public:
    HttpRequest() : _blank_line(base_sep), _path(prefixpath)
    {
    }

    void Deserialize(std::string &reqstr)
    {
        // 基本版反序列
        _req_line = Getline(reqstr);
        std::string header;
        do
        {
            header = Getline(reqstr);
            if (header.empty())
                break;
            else if (header == base_sep)
                break;
            _req_headers.push_back(header);
        } while (true);

        if (!reqstr.empty())
        {
            _body_text = reqstr; // 此时reqstr被erase到只剩正文部分
        }

        // 再进一步反序列
        ParseReqLine();   // 反序列化第一行
        ParseReqHeader(); // 反序列化报头
    }

    std::string Url()
    {
        LOG(DEBUG, "Client Want %s\n", _url.c_str());
        return _url;
    }

    std::string Path()
    {
        LOG(DEBUG, "Client Want %s\n", _path.c_str());
        return _path;
    }

    std::string Suffix()
    {
        return _suffix;
    }

    std::string Method()
    {
        LOG(DEBUG, "Client request method is %s\n", _method.c_str());
        return _method;
    }

    std::string GetResultBody()
    {
        LOG(DEBUG, "Client request method= is %s, args: %s, request path: %s\n",
             _method.c_str(),_body_text.c_str(),_path.c_str());
        return _body_text;
    }

    void Print()
    {
        std::cout << "--------------------------" << std::endl;
        std::cout << "###" << _req_line << std::endl;
        for (auto &header : _req_headers)
        {
            std::cout << "@@@" << header << std::endl;
        }
        std::cout << "*** " << _blank_line;
        std::cout << " >>> " << _body_text << std::endl;

        std::cout << "Method:" << _method << std::endl;
        std::cout << "Url:" << _url << std::endl;
        std::cout << "Version:" << _version << std::endl;

        for (auto &header_kv : _headers_kv)
        {
            std::cout << ")))" << header_kv.first << "->" << header_kv.second << std::endl;
        }
    }

    ~HttpRequest()
    {
    }

private:
    // 基本的HTTP请求格式
    std::string _req_line;                 // 第一行
    std::vector<std::string> _req_headers; // 请求报头
    std::string _blank_line;               // 空行
    std::string _body_text;                // 正文

    // 更具体的属性格式，需要更进一步反序列化
    std::string _method;
    std::string _url;
    std::string _path;
    std::string _suffix; // 资源后缀
    std::string _version;

    std::unordered_map<std::string, std::string> _headers_kv;
};

// 应答
class HttpResponse
{
public:
    HttpResponse() : _version(httpversion), _blank_line(base_sep)
    {
    }

    void AddCode(int code, const std::string &desc)
    {
        _status_code = code;
        _desc = desc;
    }

    void AddHeader(const std::string &k, const std::string &v)
    {
        _headers_kv[k] = v;
    }

    void AddBodyText(const std::string &body_text)
    {
        _resp_body_text = body_text;
    }

    std::string Serialize() // 应答序列化
    {
        // 1. 构建状态行
        _status_line = _version + spacesep + std::to_string(_status_code) + spacesep + _desc + base_sep;

        // 2. 构建应答报头
        for (auto &header : _headers_kv)
        {
            std::string header_line = header.first + line_sep + header.second + base_sep;
            _resp_headers.push_back(header_line);
        }

        // 3. 空行和正文

        // 4. 正式序列化
        std::string responsestr = _status_line;
        for (auto &line : _resp_headers)
        {
            responsestr += line;
        }
        responsestr += _blank_line;
        responsestr += _resp_body_text;

        return responsestr;
    }

    ~HttpResponse()
    {
    }

private:
    // http response base属性
    std::string _version;
    int _status_code;
    std::string _desc;
    std::unordered_map<std::string, std::string> _headers_kv;

    // 基本的HTTP应答格式
    std::string _status_line;               // 第一行
    std::vector<std::string> _resp_headers; // 应答报头
    std::string _blank_line;                // 空行
    std::string _resp_body_text;            // 正文
};



using func_t = std::function<HttpResponse(HttpRequest&)>;//你给我请求我给你应答

//服务
class HttpServer
{
private:
    std::string GetFileContent(const std::string &path)
    {
        std::ifstream in(path, std::ios::binary); // std::ios::binary 模式表示以二进制方式打开文件
        if (!in.is_open())
            return std::string();
        in.seekg(0, in.end);       // 将文件指针移动到文件末尾。
        int filesize = in.tellg(); // 返回当前文件指针的位置，即文件的大小。filesize存储文件的大小。
        in.seekg(0, in.beg);       // 将文件指针移动到文件开头，准备读取文件内容。

        std::string content;
        content.resize(filesize);
        in.read((char *)content.c_str(), filesize);

        in.close();
        return content;
    }

public:
    HttpServer()
    {
        // 后缀
        _mime_type.insert(std::make_pair(".html", "text/html"));
        _mime_type.insert(std::make_pair(".jpg", "image/jpeg"));
        _mime_type.insert(std::make_pair(".png", "image/png"));
        _mime_type.insert(std::make_pair(".default", "text/html"));

        // 状态码
        _code_to_desc.insert(std::make_pair(100, "Continue"));
        _code_to_desc.insert(std::make_pair(200, "OK"));
        _code_to_desc.insert(std::make_pair(201, "Created"));
        _code_to_desc.insert(std::make_pair(301, "Moved Permanently"));
        _code_to_desc.insert(std::make_pair(302, "Found"));
        _code_to_desc.insert(std::make_pair(404, "Not Found"));
    }

    //#define TEST
    std::string HandlerHttpRequest(std::string &reqstr) // req曾被序列化过
    {
#ifdef TEST
        std::cout << "---------------------------" << std::endl;
        std::cout << reqstr;

        std::string responsestr = "HTTP/1.1 200 OK\r\n";
        responsestr += "Content-Type: text/html\r\n";
        responsestr += "\r\n";
        responsestr += "<html><h1>hello linx, hello bit!</h1><html>";

        return responsestr;

#else


        HttpRequest req;
        HttpResponse resp;

        req.Deserialize(reqstr); // 反序列化
        //req.Method();

        if (req.Path() == "wwwroot/redir") // 重定向
        {
            //处理重定向
            std::string redir_path = "https://www.bilibili.com";
            resp.AddCode(301,_code_to_desc[301]);
            resp.AddHeader("Location", redir_path);

        }
        else if(!req.GetResultBody().empty())
        {
            if(IsServiceExists(req.Path()))
            {
               resp =  _service_list[req.Path()](req);  
               //调用与请求路径匹配的服务处理函数，并将请求对象传递给该函数，从而生成对应的 HTTP 响应。

            }
        }

        else
        {
            //最基本的上层处理，处理静态资源 
            std::string content = GetFileContent(req.Path());

            if (content.empty())
            {
                content = GetFileContent("wwwroot/404.html");
                resp.AddCode(404, _code_to_desc[404]);
                resp.AddHeader("Content-Length", std::to_string(content.size()));
                resp.AddHeader("Content-Type", _mime_type[".html"]); // mime_type里存放着不同后缀，用于告诉浏览器你要请求的格式
                resp.AddBodyText(content);
            }
            else // 添加报头
            {
                resp.AddCode(200, _code_to_desc[200]);
                resp.AddHeader("Content-Length", std::to_string(content.size()));
                resp.AddHeader("Content-Type", _mime_type[req.Suffix()]); // mime_type里存放着不同后缀，用于告诉浏览器你要请求的格式
                resp.AddHeader("Set-Cookie", "username = kjy");
                //resp.AddHeader("Set-Cookie:", "password = 666");
                resp.AddBodyText(content);
            }
        }

        return resp.Serialize();

#endif
    }
    void InsertService(const std::string &servicename, func_t f)
    {
        std::string s = prefixpath + servicename;
        _service_list[s] = f;
    }

    bool IsServiceExists(const std::string &servicename)
    {
        auto iter = _service_list.find(servicename);
        if(iter == _service_list.end())return false;
        else return true;
    }


    ~HttpServer()
    {
    }

private:
    std::unordered_map<std::string, std::string> _mime_type;
    std::unordered_map<int, std::string> _code_to_desc;
    std::unordered_map<std::string, func_t> _service_list;
};