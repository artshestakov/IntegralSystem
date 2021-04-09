#pragma once
#ifndef _ISTCPMESSAGE_H_INCLUDED
#define _ISTCPMESSAGE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "rapidjson/document.h"
#include "ISTcpClient.h"
//-----------------------------------------------------------------------------
class ISTcpMessage
{
public:
    ISTcpMessage();
    ~ISTcpMessage();

    std::string GetErrorString() const;
    void SetErrorString(const std::string &error_string);
    bool IsValid() const;

    std::string Type; //Тип сообщения
    rapidjson::Document Parameters; //Параметры
    ISUInt64 MSecParse; //Время парсинга
    size_t Size;
    size_t ChunkCount; //Количетсво чанков
    ISTcpClient *TcpClient; //Клиент, от которого пришло это сообщение

private:
    std::string ErrorString;
    bool Valid; //Флаг валидности сообщения
};
//-----------------------------------------------------------------------------
#endif
