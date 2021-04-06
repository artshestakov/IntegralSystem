#pragma once
#ifndef _ISSTRUCTS_H_INCLUDED
#define _ISSTRUCTS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
struct ISDate
{
    ISDate()
        : Day(0), Month(0), Year(0) { }

    ISDate(unsigned short day, unsigned short month, unsigned short year)
        : Day(day), Month(month), Year(year) { }

    bool IsNull() const
    {
        return Day == 0 && Month == 0 && Year == 0;
    }

    bool operator<(const ISDate &d)
    {
        if (Year < d.Year)
        {
            return true;
        }
        else if (Year == d.Year)
        {
            if (Month < d.Month)
            {
                return true;
            }
            else if (Month == d.Month)
            {
                if (Day < d.Day)
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool operator>(const ISDate &d)
    {
        if (Year > d.Year)
        {
            return true;
        }
        else if (Year == d.Year)
        {
            if (Month > d.Month)
            {
                return true;
            }
            else if (Month == d.Month)
            {
                if (Day > d.Day)
                {
                    return true;
                }
            }
        }
        return false;
    }

    unsigned short Day;
    unsigned short Month;
    unsigned short Year;
};
//-----------------------------------------------------------------------------
struct ISTime
{
    ISTime()
        : Hour(0), Minute(0), Second(0), Milliseconds(0) { }

    ISTime(unsigned short hour, unsigned short minute, unsigned short second, unsigned short milliseconds)
        : Hour(hour), Minute(minute), Second(second), Milliseconds(milliseconds) { }

    unsigned short Hour;
    unsigned short Minute;
    unsigned short Second;
    unsigned short Milliseconds;
};
//-----------------------------------------------------------------------------
struct ISDateTime
{
    ISDateTime() { }

    ISDateTime(const ISDate &date, const ISTime &time)
        : Date(date), Time(time) { }

    ISDate Date;
    ISTime Time;
};
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
#endif
