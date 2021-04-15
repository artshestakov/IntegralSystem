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

    static ISDate CurrentDate();

    bool IsNull() const;
    std::string ToString() const;

    bool operator<(const ISDate &Date);
    bool operator>(const ISDate &Date);

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

    bool IsNull() const; //�������� ������� �� �������
    std::string ToString() const; //�������� ����� � ������
    size_t ToMSec() const; //�������� ����� � �������������

    bool operator<(const ISTime &Time);
    bool operator>(const ISTime &Time);

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

    static ISDateTime CurrentDateTime();

    bool IsNull() const;
    std::string ToString() const;

    bool operator<(const ISDateTime &DateTime);
    bool operator>(const ISDateTime &DateTime);

    ISDate Date;
    ISTime Time;
};
//-----------------------------------------------------------------------------
#endif
