#pragma once
#ifndef _ISSTRUCTS_H_INCLUDED
#define _ISSTRUCTS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISDateTime.h"
#include "ISNamespace.h"
#include "ISConstants.h"
#include "ISTypedefs.h"
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
#ifdef WIN32
    unsigned ThreadID;
#else
    pthread_t ThreadID;
#endif
    int ParameterCount;
};
//-----------------------------------------------------------------------------
struct ISConfigurationInfo
{
    ISConfigurationInfo()
        : Type(ISNamespace::ConfigurationType::Unknown)
    {

    }

    ISNamespace::ConfigurationType Type; //��� ������������
    ISString Name; //������������
    ISString UID; //�������������
    ISString LocalName; //��������� ������������
    ISString DesktopForm; //��� ������ �������� �����
    ISDate DateExpired; //����, ����� ������� ��������� ����������� �� ������
    ISString LogoName;
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
struct ISLockedIP
{
    ISLockedIP(const std::string &ip_address)
        : IPAddress(ip_address),
        AttemptLeft(CARAT_AUTH_FAIL_LIMIT),
        DateTimeUnlock(ISDateTime::GetCurrentUnixtime() + (CARAT_AUTH_MINUTE_LOCK * 60))
    { }

    bool IsLock() const
    {
        return AttemptLeft == 0;
    }

    std::string IPAddress; //IP-�����
    size_t AttemptLeft; //�������� �������
    uint64_t DateTimeUnlock; //���� � ����� �������������
};
//-----------------------------------------------------------------------------
struct CGSectionItem
{
    std::string Function;
    std::string Description;
};
//-----------------------------------------------------------------------------
struct CGSection
{
    std::string Name;
    std::string ClassName;
    std::vector<CGSectionItem> Items;
};
//-----------------------------------------------------------------------------
struct ISResourceItem
{
    ISResourceItem(const unsigned char *data, int size, const char *name)
        : Data(data), Size(size), Name(name)
    {

    }

    const unsigned char *Data;
    int Size;
    const char *Name;
};
//-----------------------------------------------------------------------------
#endif
