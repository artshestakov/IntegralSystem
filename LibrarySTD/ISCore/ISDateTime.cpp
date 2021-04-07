#include "ISDateTime.h"
#include "ISAlgorithm.h"
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
bool ISDate::operator<(const ISDate &d)
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
//-----------------------------------------------------------------------------
bool ISDate::operator>(const ISDate &d)
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
ISDateTime ISDateTime::CurrentDateTime()
{
#ifdef WIN32
    SYSTEMTIME ST;
    GetLocalTime(&ST);
    return{ { ST.wDay, ST.wMonth, ST.wYear },{ ST.wHour, ST.wMinute, ST.wSecond, ST.wMilliseconds } };
#else
    struct timeval TimeValue;
    gettimeofday(&TimeValue, NULL);
    struct tm *ST = localtime(&TimeValue.tv_sec);
    return{ (unsigned short)ST->tm_mday, (unsigned short)(ST->tm_mon + 1), (unsigned short)(ST->tm_year + 1900),
        (unsigned short)ST->tm_hour, (unsigned short)ST->tm_min, (unsigned short)ST->tm_sec, (unsigned short)(TimeValue.tv_usec / 1000) };
#endif
}
//-----------------------------------------------------------------------------
