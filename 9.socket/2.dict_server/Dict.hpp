#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <unistd.h>

#include "Log.hpp"

using namespace log_ns;

const static std::string sep = ": ";

class Dict
{
private:
    void LoadDict(const std::string &path)
    {
        std::ifstream in(path);
        if(!in.is_open())
        {
            LOG(FATAL, "open %s failed!\n", path.c_str());
            exit(1);
        }

        std::string line;
        while (std::getline(in, line))
        {
            LOG(DEBUG, "load info: %s, success\n", line.c_str());
            if(line.empty())continue;
            auto pos = line.find(sep);
            if(pos == std::string::npos)continue;
            
            std::string key = line.substr(0,pos);
            if(key.empty()) continue;

            std::string value = line.substr(pos + sep.size());
            if(value.empty())continue;

            _dict.insert(std::make_pair(key,value));
        }
        LOG(INFO, "load %s done\n", path.c_str());
        in.close();
    }





public:
    Dict(const std::string &dict_path): _dict_path(dict_path)
    {
        LoadDict(_dict_path);
    }
    std::string Translate(std::string word)
    {
        if(word.empty())return "NONE";
        auto iter = _dict.find(word);
        if(iter == _dict.end())return "NONE";
        else return iter->second;
    }
    ~Dict()
    {}
private:
    std::unordered_map <std::string, std::string> _dict;
    std::string _dict_path;
};
