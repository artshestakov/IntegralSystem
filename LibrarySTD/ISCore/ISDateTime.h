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

    static ISDate CurrentDate(); //Получить текущую дату

    bool IsNull() const; //Проверка даты на пустоту
    std::string ToString() const; //Привести дату к строке

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

    static ISTime CurrentTime(); //Получить текущее время
    static ISTime FromSecond(unsigned int Seconds); //Перевод секунд во время

    bool IsNull() const; //Проверка времени на пустоту
    std::string ToString() const; //Привести время в строку
    size_t ToMSec() const; //Привести время к миллисекундам

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

    static ISDateTime CurrentDateTime(); //Получить текущую дату и время

    bool IsNull() const; //Проверка даты и времени на пустоту
    std::string ToString() const; //Привести дату и время к строке

    bool operator<(const ISDateTime &DateTime) const;
    bool operator>(const ISDateTime &DateTime) const;

    ISDate Date;
    ISTime Time;
};
//-----------------------------------------------------------------------------
#endif
