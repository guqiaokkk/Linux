#pragma once

#include <memory>

#include "Protocol.hpp"

class NetCal
{
public:
    NetCal()
    {
    }

    std::shared_ptr<Response> Calculator(std::shared_ptr<Request> req)
    {
        auto resp = Factory::BuildResponseDefault();
        switch (req->Oper())
        {
        case '+':
            resp->_result = req->X() + req->Y();
            break;
        case '-':
            resp->_result = req->X() - req->Y();

            break;
        case '*':
            resp->_result = req->X() * req->Y();

            break;
        case '/':
        {
            if (req->Y() == 0)
            {
                resp->_code = 1;
                resp->_desc = "div zero";
            }
            else
            {
                resp->_result = req->X() / req->Y();
            }
        }
        break;
        case '%':
        {
            if (req->Y() == 0)
            {
                resp->_code = 1;
                resp->_desc = "mod zero";
            }
            else
            {
                resp->_result = req->X() % req->Y();
            }
        }
        break;
        default:
        {
            resp->_code = 3;
            resp->_desc = "illegal operation";
        }
        break;
        }
        return resp;
    }

    ~NetCal()
    {
    }

private:
};