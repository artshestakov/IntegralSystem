#pragma once
#ifndef _ISTCPCONNECTOR_H_INCLUDED
#define _ISTCPCONNECTOR_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISTcpConnector
{
public:
    static ISTcpConnector& Instance();

    const std::string& GetErrorString() const; //Получить текствоое описание ошибки
    const ISSocket& GetSocket() const; //Получить сокет
    bool IsConnected() const; //Проверить наличие соединения
    bool Connect(const std::string &Host, unsigned short Port); //Подключение
    void Disconnect(); //Отключение

private:
    ISTcpConnector();
    ~ISTcpConnector();
    ISTcpConnector(const ISTcpConnector &) = delete;
    ISTcpConnector(ISTcpConnector &&) = delete;
    ISTcpConnector& operator=(const ISTcpConnector &) = delete;
    ISTcpConnector& operator=(ISTcpConnector &&) = delete;

private:
    std::string ErrorString;
    ISSocket Socket;
};
//-----------------------------------------------------------------------------
#endif
