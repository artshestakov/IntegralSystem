#pragma once
#ifndef _ISTCPANSWER_H_INCLUDED
#define _ISTCPANSWER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "rapidjson/document.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISTcpAnswer
{
public:
	ISTcpAnswer(ISSocket socket_client);
    ~ISTcpAnswer();

    bool IsError() const;
    const std::string& GetErrorString() const;
    void SetError(const std::string &error_string);
	ISSocket GetSocketClient() const; //Получить дескриптор клиентского сокета
    std::string ToJson() const;

    rapidjson::Document Parameters;
    
private:
    std::string ErrorString;
    bool Error;
	ISSocket SocketClient; //Клиентский сокет
};
//-----------------------------------------------------------------------------
#endif
