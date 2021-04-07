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

    bool operator<(const ISDate &d);
    bool operator>(const ISDate &d);

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
    ISTime(unsigned short hour, unsigned short minute, unsigned short second, unsigned short milliseconds);

    static ISTime CurrentTime();

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

    static ISDateTime CurrentDateTime();

    ISDate Date;
    ISTime Time;
};
//-----------------------------------------------------------------------------
#endif
