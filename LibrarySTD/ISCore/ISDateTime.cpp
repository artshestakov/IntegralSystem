#include "ISDateTime.h"
#include "ISAlgorithm.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISDate::ISDate()
    : Day(0), Month(0), Year(0)
{

}
//-----------------------------------------------------------------------------
ISDate::ISDate(unsigned short day, unsigned short month, unsigned short year)
    : Day(day), Month(month), Year(year)
{

}
//-----------------------------------------------------------------------------
ISDate ISDate::CurrentDate()
{
    ISDateTime DateTime = ISDateTime::CurrentDateTime();
    return{ DateTime.Date.Day, DateTime.Date.Month, DateTime.Date.Year };
}
//-----------------------------------------------------------------------------
bool ISDate::IsNull() const
{
    return Day == 0 && Month == 0 && Year == 0;
}
//-----------------------------------------------------------------------------
std::string ISDate::ToString() const
{
    std::string Result;
    if (!IsNull())
    {
        Result = ISAlgorithm::StringF("%02d.%02d.%d", Day, Month, Year);
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool ISDate::operator<(const ISDate &Date)
{
    if (Year < Date.Year)
    {
        return true;
    }
    else if (Year == Date.Year)
    {
        if (Month < Date.Month)
        {
            return true;
        }
        else if (Month == Date.Month)
        {
            if (Day < Date.Day)
            {
                return true;
            }
        }
    }
    return false;
}
//-----------------------------------------------------------------------------
bool ISDate::operator>(const ISDate &Date)
{
    if (Year > Date.Year)
    {
        return true;
    }
    else if (Year == Date.Year)
    {
        if (Month > Date.Month)
        {
            return true;
        }
        else if (Month == Date.Month)
        {
            if (Day > Date.Day)
            {
                return true;
            }
        }
    }
    return false;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISTime::ISTime()
    : Hour(0), Minute(0), Second(0), Milliseconds(0)
{

}
//-----------------------------------------------------------------------------
ISTime::ISTime(unsigned short hour, unsigned short minute, unsigned short second, unsigned short milliseconds)
    : Hour(hour), Minute(minute), Second(second), Milliseconds(milliseconds)
{

}
//-----------------------------------------------------------------------------
ISTime ISTime::CurrentTime()
{
    ISDateTime DateTime = ISDateTime::CurrentDateTime();
    return{ DateTime.Time.Hour, DateTime.Time.Minute, DateTime.Time.Second, DateTime.Time.Milliseconds };
}
//-----------------------------------------------------------------------------
bool ISTime::IsNull() const
{
    return Hour == 0 && Minute == 0 && Second == 0 && Milliseconds == 0;
}
//-----------------------------------------------------------------------------
std::string ISTime::ToString() const
{
    std::string Result;
    if (!IsNull())
    {
        Result = ISAlgorithm::StringF("%02d:%02d:%02d", Hour, Minute, Second);
    }
    return Result;
}
//-----------------------------------------------------------------------------
size_t ISTime::ToMSec() const
{
    return (Hour * 3600000) + (Minute * 60000) + (Second * 1000) + Milliseconds;
}
//-----------------------------------------------------------------------------
bool ISTime::operator<(const ISTime &Time)
{
    size_t t1 = ToMSec(), t2 = Time.ToMSec();
    if (t1 == t2)
    {
        return false;
    }
    return t1 < t2;
}
//-----------------------------------------------------------------------------
bool ISTime::operator>(const ISTime &Time)
{
    size_t t1 = ToMSec(), t2 = Time.ToMSec();
    if (t1 == t2)
    {
        return false;
    }
    return t1 > t2;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ISDateTime::ISDateTime()
{

}
//-----------------------------------------------------------------------------
ISDateTime::ISDateTime(const ISDate &date, const ISTime &time)
    : Date(date), Time(time)
{

}
//-----------------------------------------------------------------------------
ISDateTime::ISDateTime(unsigned short day, unsigned short month, unsigned short year, unsigned short hour, unsigned short minute, unsigned short second, unsigned short milliseconds)
    : ISDateTime(ISDate(day, month, year), ISTime(hour, minute, second, milliseconds))
{

}
//-----------------------------------------------------------------------------
ISDateTime ISDateTime::CurrentDateTime()
{
#ifdef WIN32
    SYSTEMTIME ST;
    GetLocalTime(&ST);
    return{ { ST.wDay, ST.wMonth, ST.wYear },
        { ST.wHour, ST.wMinute, ST.wSecond, ST.wMilliseconds } };
#else
    struct timeval TimeValue;
    gettimeofday(&TimeValue, NULL);
    struct tm *ST = localtime(&TimeValue.tv_sec);
    return { { (unsigned short)ST->tm_mday, (unsigned short)(ST->tm_mon + 1), (unsigned short)(ST->tm_year + 1900) },
        { (unsigned short)ST->tm_hour, (unsigned short)ST->tm_min, (unsigned short)ST->tm_sec, (unsigned short)(TimeValue.tv_usec / 1000) } };
#endif
}
//-----------------------------------------------------------------------------
bool ISDateTime::IsNull() const
{
    return Date.IsNull() && Time.IsNull();
}
//-----------------------------------------------------------------------------
std::string ISDateTime::ToString() const
{
    std::string Result;
    if (!IsNull())
    {
        Result = Date.ToString() + ' ' + Time.ToString();
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool ISDateTime::operator<(const ISDateTime &DateTime)
{
    return Date < DateTime.Date && Time < DateTime.Time;
}
//-----------------------------------------------------------------------------
bool ISDateTime::operator>(const ISDateTime &DateTime)
{
    return Date > DateTime.Date && Time > DateTime.Time;
}
//-----------------------------------------------------------------------------
