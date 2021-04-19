#pragma once
#ifndef _ISTCPWORKER_H_INCLUDED
#define _ISTCPWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
#include "ISTcpMessage.h"
#include "ISTcpAnswer.h"
#include "ISQuery.h"
#include "ISMetaData.h"
//-----------------------------------------------------------------------------
class ISTcpWorker
{
public:
    ISTcpWorker();
    ~ISTcpWorker();

    bool GetBusy(); //�������� "���������" �������
    void SetMessage(ISTcpMessage *TcpMessage); //���������� ��������� �� ���������
    void Start(); //��������� ������
    void Shutdown(); //���������� ������

private:
    void SetRunning(bool is_running);
    bool GetRunning();
    void Process();
    bool Execute(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool CheckIsNull(ISTcpMessage *TcpMessage, const char *ParameterName);
    bool ErrorQuery(const std::string &LocalError, ISQuery &SqlQuery);
    void Protocol(unsigned int UserID, const char *ActionUID, const std::string &TableName = std::string(), const std::string &TableLocalName = std::string(), unsigned int ObjectID = 0, const std::string &Information = std::string());
    unsigned int ExtractVersionFile(const std::string &FileName);
    PMetaTable* GetMetaTable(const std::string &TableName); //�������� ��������� �� ����-�������
    bool UserPasswordExist(unsigned int UserID, bool &Exist); //�������� ������� ������ � ������������

private:
    bool Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool Sleep(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool GetLastClient(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    bool UserPasswordExist(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);

private:
    std::string ErrorString;
    bool IsBusy;
    bool IsRunning;
    bool IsFinished;
    ISTcpMessage *CurrentMessage;
    PGconn *DBConnection; //��������� �� ����������
    ISQuery *qProtocol;
    ISCriticalSection CriticalSection;
    ISCriticalSection CSRunning;

    //������ TCP-�������
    typedef bool (ISTcpWorker::*TCPFunction)(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
    std::map<std::string, TCPFunction> MapFunction;
};
//-----------------------------------------------------------------------------
#endif
