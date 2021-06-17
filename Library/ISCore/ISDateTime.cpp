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
ISDate ISDate::FromString(const char *String, const char *Format)
{
    int Day = 0, Month = 0, Year = 0;
    sscanf(String, Format, &Day, &Month, &Year);
    return ISDate((unsigned short)Day, (unsigned short)Month, (unsigned short)Year);
}
//-----------------------------------------------------------------------------
bool ISDate::IsNull() const
{
    return Day == 0 && Month == 0 && Year == 0;
}
//-----------------------------------------------------------------------------
std::string ISDate::ToString() const
{
    return ISAlgorithm::StringF("%02d.%02d.%04d", Day, Month, Year);
}
//-----------------------------------------------------------------------------
ISDate ISDate::AddDays(int Days)
{
    //Если дни не указаны или дата пустая - возвращаем пустую дату
    if (Days == 0 || IsNull())
    {
        return ISDate();
    }

    //Получаем текущую дату и модернизируем её
    time_t Now = time(NULL);
    tm *TM = localtime(&Now);
    TM->tm_mday = Day;
    TM->tm_mon = Month - 1;
    TM->tm_year = Year - 1900;
    time_t Time = mktime(TM);

    //Проверяем, нужно добавлять дни или отнимать
    bool IsAdd = Days > 0;
    if (!IsAdd) //Если нужно отнимать - меняем знак
    {
        Days = -Days;
    }

    for (int i = 0; i < Days; ++i)
    {
        IsAdd ? Time += (int)DAY_IN_SECONDS : Time -= (int)DAY_IN_SECONDS;
    }
    return ISDateTime::FromUnixTime(Time).Date;
}
//-----------------------------------------------------------------------------
bool ISDate::operator<(const ISDate &Date) const
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
bool ISDate::operator>(const ISDate &Date) const
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
bool ISDate::operator==(const ISDate &Date) const
{
    return Day == Date.Day && Month == Date.Month && Year == Date.Year;
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
ISTime ISTime::FromSecond(unsigned int Seconds)
{
    unsigned int Hour = Seconds / 3600,
        Minute = (Seconds - Hour * 3600) / 60,
        Second = int(Seconds % 60);
    return ISTime((unsigned short)Hour, (unsigned short)Minute, (unsigned short)Second);
}
//-----------------------------------------------------------------------------
bool ISTime::IsNull() const
{
    return Hour == 0 && Minute == 0 && Second == 0 && Milliseconds == 0;
}
//-----------------------------------------------------------------------------
std::string ISTime::ToString() const
{
    return ISAlgorithm::StringF("%02d:%02d:%02d", Hour, Minute, Second);
}
//-----------------------------------------------------------------------------
size_t ISTime::ToMSec() const
{
    return (Hour * 3600000) + (Minute * 60000) + (Second * 1000) + Milliseconds;
}
//-----------------------------------------------------------------------------
bool ISTime::operator<(const ISTime &Time) const
{
    size_t t1 = ToMSec(), t2 = Time.ToMSec();
    if (t1 == t2)
    {
        return false;
    }
    return t1 < t2;
}
//-----------------------------------------------------------------------------
bool ISTime::operator>(const ISTime &Time) const
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
uint64_t ISDateTime::GetCurrentUnixtime()
{
    return std::chrono::seconds(std::chrono::seconds(std::time(NULL))).count();
}
//-----------------------------------------------------------------------------
ISDateTime ISDateTime::FromUnixTime(uint64_t Unixtime)
{
    time_t Time = Unixtime;
    struct tm *TM = localtime(&Time);
    return ISDateTime((unsigned short)TM->tm_mday, (unsigned short)TM->tm_mon + 1, (unsigned short)TM->tm_year + 1900,
        (unsigned short)TM->tm_hour, (unsigned short)TM->tm_min, (unsigned short)TM->tm_sec);
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
bool ISDateTime::operator<(const ISDateTime &DateTime) const
{
    if (Date < DateTime.Date)
    {
        return true;
    }
    else if (Date == DateTime.Date)
    {
        if (Time < DateTime.Time)
        {
            return true;
        }
    }
    return false;
}
//-----------------------------------------------------------------------------
bool ISDateTime::operator>(const ISDateTime &DateTime) const
{
    if (Date > DateTime.Date)
    {
        return true;
    }
    else if (Date == DateTime.Date)
    {
        if (Time > DateTime.Time)
        {
            return true;
        }
    }
    return false;
}
//-----------------------------------------------------------------------------
