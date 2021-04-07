#pragma once
#ifndef _ISTCPANSWER_H_INCLUDED
#define _ISTCPANSWER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "document.h"
//-----------------------------------------------------------------------------
class ISTcpAnswer
{
public:
    ISTcpAnswer(SOCKET socket_client);
    ~ISTcpAnswer();

    bool IsError() const;
    std::string GetErrorString() const;
    void SetError(const std::string &error_string);
    SOCKET GetSocketClient() const; //Получить дескриптор клиентского сокета
    std::string ToJson() const;

    rapidjson::Document Parameters;
    
private:
    std::string ErrorString;
    bool Error;
    SOCKET SocketClient; //Клиентский сокет
};
//-----------------------------------------------------------------------------
#endif
