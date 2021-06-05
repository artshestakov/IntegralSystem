#pragma once
#ifndef _ISTCPSERVER_H_INCLUDED
#define _ISTCPSERVER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
#include "ISTcpClient.h"
#include "ISTcpMessage.h"
#include "ISTcpWorker.h"
//-----------------------------------------------------------------------------
class ISTcpServer
{
public:
    static ISTcpServer& Instance();

    const std::string& GetErrorString() const;
    
    bool Start();
    void Stop();

private:
    void WorkerAcceptor(); //Поток приёма соединений
    void WorkerReader(ISTcpClient *TcpClient); //Поток чтения данных
    void WorkerBalancer(); //Поток распределения сообщений
    void WorkerAnswer(); //Поток ответов

    bool ParseMessage(const char *Buffer, size_t BufferSize, ISTcpMessage *TcpMessage);
    void CloseSocket(ISTcpClient *TcpClient);
	void CloseSocket(ISSocket Socket);
    bool GetIsRunning(); //Проверка работы сервера
    
private:
    ISTcpServer();
    ~ISTcpServer();
    ISTcpServer(const ISTcpServer&) = delete;
    ISTcpServer(ISTcpServer&&) = delete;
    ISTcpServer& operator=(const ISTcpServer&) = delete;
    ISTcpServer& operator=(ISTcpServer&&) = delete;

private:
    std::string ErrorString;
	ISSocket SocketServer;
    bool IsRunning;
    std::vector<ISTcpWorker*> Workers;
    unsigned int WorkerCount;
    ISCriticalSection CSRunning;
};
//-----------------------------------------------------------------------------
#endif
