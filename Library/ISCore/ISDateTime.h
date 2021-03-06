#pragma once
#ifndef _ISDATETIME_H_INCLUDED
#define _ISDATETIME_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
struct ISDate
{
    ISDate();
    ISDate(unsigned short day, unsigned short month, unsigned short year);

    static ISDate CurrentDate(); //�������� ������� ����
    static ISDate FromString(const char *String, const char *Format);

    bool IsNull() const; //�������� ���� �� �������
    std::string ToString() const; //�������� ���� � ������
    ISDate AddDays(int Days); //�������� ��� � ����

    bool operator<(const ISDate &Date) const;
    bool operator>(const ISDate &Date) const;
    bool operator==(const ISDate &Date) const;

    unsigned short Day;
    unsigned short Month;
    unsigned short Year;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct ISTime
{
    ISTime();
    ISTime(unsigned short hour, unsigned short minute, unsigned short second = 0, unsigned short milliseconds = 0);

    static ISTime CurrentTime(); //�������� ������� �����
    static ISTime FromSecond(unsigned int Seconds); //������� ������ �� �����

    bool IsNull() const; //�������� ������� �� �������
    std::string ToString() const; //�������� ����� � ������
    size_t ToMSec() const; //�������� ����� � �������������

    bool operator<(const ISTime &Time) const;
    bool operator>(const ISTime &Time) const;

    unsigned short Hour;
    unsigned short Minute;
    unsigned short Second;
    unsigned short Milliseconds;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct ISDateTime
{
    ISDateTime();
    ISDateTime(const ISDate &date, const ISTime &time);
    ISDateTime(unsigned short day, unsigned short month, unsigned short year, unsigned short hour, unsigned short minute, unsigned short second = 0, unsigned short milliseconds = 0);

    static ISDateTime CurrentDateTime(); //�������� ������� ���� � �����
    static uint64_t GetCurrentUnixtime(); //�������� ������� ����� ������� � ������� Unixtime
    static ISDateTime FromUnixTime(uint64_t Unixtime); //�������������� unixtime � ���� � �����

    bool IsNull() const; //�������� ���� � ������� �� �������
    std::string ToString() const; //�������� ���� � ����� � ������

    bool operator<(const ISDateTime &DateTime) const;
    bool operator>(const ISDateTime &DateTime) const;

    ISDate Date;
    ISTime Time;
};
//-----------------------------------------------------------------------------
#endif
