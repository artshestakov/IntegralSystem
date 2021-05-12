#pragma once
#ifndef _ISSTRUCTS_H_INCLUDED
#define _ISSTRUCTS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISDateTime.h"
#include "ISNamespace.h"
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
struct ISMetaType
{
    ISMetaType(const std::string& type_name, ISNamespace::FieldType type, const std::string& type_db, const std::string& control_widget, const std::string& search_condition_widget, bool search_allowed)
        : TypeName(type_name), TypeField(type), TypeDB(type_db), ControlWidget(control_widget), SearchConditionWidget(search_condition_widget), SearchAllowed(search_allowed)
    { }

    std::string TypeName; //������������ ����
    ISNamespace::FieldType TypeField; //��� ������� � �������
    std::string TypeDB; //��� ������ � ����
    std::string ControlWidget; //��� ���� ��������������
    std::string SearchConditionWidget; //��� ���������� �������
    bool SearchAllowed; //����������� ������ �� ����
};
//-----------------------------------------------------------------------------
struct ISSqlPrepare
{
    std::string SqlText;
    std::string Hash;
    unsigned ThreadID;
    int ParameterCount;
};
//-----------------------------------------------------------------------------
struct ISConfigurationInfo
{
    std::string Name; //������������
    std::string UID; //�������������
    std::string LocalName; //��������� ������������
    std::string DesktopForm; //��� ������ �������� �����
    ISDate DateExpired; //����, ����� ������� ��������� ����������� �� ������
    std::string LogoName;
};
//-----------------------------------------------------------------------------
struct ISFileInfo
{
    ISFileInfo()
        : Size(0) { }

    std::string Path; //���� � �����
    std::string Name; //��� �����
    ISDateTime DateTimeEdit; //���� ��������� �����
    unsigned long Size; //������ �����
};
//-----------------------------------------------------------------------------
struct ISLocal
{
    ISLocal()
        : Number(0) { }

    char Value[256];
    int Number;
};
//-----------------------------------------------------------------------------
#endif
