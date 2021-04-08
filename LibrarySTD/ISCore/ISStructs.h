#pragma once
#ifndef _ISSTRUCTS_H_INCLUDED
#define _ISSTRUCTS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISAlgorithm.h"
#include "ISDateTime.h"
//-----------------------------------------------------------------------------
struct ISConfigParameter
{
    std::string TemplateName; //��� �������
    std::string SectionName; //��� ������
    std::string Name; //��� ���������
    std::string Type; //��� ���������
    bool NotNull; //�������� ���������� ��� ����������
    std::string DefaultValue; //�������� �� ���������
    int Minimum; //����������� �������� (��� �������� ����������)
    int Maximum; //������������ �������� (��� �������� ����������)
};
//-----------------------------------------------------------------------------
struct ISConnectOptionDB
{
    ISConnectOptionDB() : Port(0) { }

    ISConnectOptionDB(const std::string& host, unsigned short port, const std::string& name, const std::string& login, const std::string& password)
        : Host(host), Port(port), Name(name), Login(login), Password(password) { }

    std::string Host; //����� �������
    unsigned short Port; //����
    std::string Name; //��� ���� ������
    std::string Login; //����� ������������
    std::string Password; //������
};
//-----------------------------------------------------------------------------
struct ISResourceFile
{
    ISResourceFile()
        : Size(0), Data(nullptr) { }

    ISResourceFile(const std::string &name, unsigned long size, char *data)
        : Name(name), Size(size), Data(data) { }

    std::string Name;
    unsigned long Size;
    char *Data;
};
//-----------------------------------------------------------------------------
struct ISTcpClientInfo
{
    std::string IPAddress; //����� �������
    ISDateTime DTConnected; //���� � ����� �����������
};
//-----------------------------------------------------------------------------
#endif
