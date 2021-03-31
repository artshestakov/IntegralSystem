#pragma once
#ifndef _ISTCPWORKER_H_INCLUDED
#define _ISTCPWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
#include "ISTcpMessage.h"
#include "ISTcpAnswer.h"
#include "ISQuery.h"
#include <libpq-fe.h>
//-----------------------------------------------------------------------------
class ISTcpWorker
{
public:
    ISTcpWorker();
    ~ISTcpWorker();

    bool GetBusy(); //Получить "занятость" воркера
    void SetMessage(ISTcpMessage *TcpMessage); //Установить сообщение на обработку
    void Start(); //Запустить воркер
    void Shutdown(); //Остановить воркер

private:
    void SetRunning(bool is_running);
    bool GetRunning();
    void Process();
    bool Execute(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool CheckIsNull(ISTcpMessage *TcpMessage, const char *ParameterName);
    bool ErrorQuery(const std::string &LocalError, ISQuery &SqlQuery);

private:
    bool Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool Sleep(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);

private:
    std::string ErrorString;
    bool IsBusy;
    bool IsRunning;
    bool IsFinished;
    ISTcpMessage *CurrentMessage;
    PGconn *DBConnection; //Указатель на соединение
    ISCriticalSection CriticalSection;
    ISCriticalSection CSRunning;

    //Список TCP-функций
    typedef bool (ISTcpWorker::*TCPFunction)(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    std::map<std::string, TCPFunction> MapFunction;
};
//-----------------------------------------------------------------------------
#endif
