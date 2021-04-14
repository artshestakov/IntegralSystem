#pragma once
#ifndef _ISDATABASE_H_INCLUDED
#define _ISDATABASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISStructs.h"
#include "ISTypedefs.h"
#include "libpq-fe.h"
//-----------------------------------------------------------------------------
class ISDatabase
{
public:
    static ISDatabase& Instance();

    std::string GetErrorString() const; //�������� �������� ��������� ������

    PGconn* GetDB(const std::string &ConnectionName); //�������� ������ �� ��������� �� �� ����� ����������
    ISConnectOptionDB GetOption(const std::string &ConnectionName); //�������� ��������� ����������� � ��

    bool Connect(const std::string &ConnectionName, const ISConnectOptionDB &ConnectOptionDB); //����������� � ��
    bool Connect(const std::string &ConnectionName, const std::string &Host, unsigned short Port, const std::string &Database, const std::string &Login, const std::string &Password); //����������� � ��
    void Disconnect(const std::string &ConnectionName); //���������� �� ��
    void DisconnectAll(); //���������� �� ���� ��

private:
    ISDatabase();
    ~ISDatabase();
    ISDatabase(const ISDatabase&) = delete;
    ISDatabase(ISDatabase&&) = delete;
    ISDatabase& operator=(const ISDatabase&) = delete;
    ISDatabase& operator=(ISDatabase&&) = delete;

private:
    std::string ErrorString;
    std::map<std::string, PGconn*> ConnectionsMap;
    ISCriticalSection CriticalSection; //����������� ������ ��� �������������
};
//-----------------------------------------------------------------------------
#endif
