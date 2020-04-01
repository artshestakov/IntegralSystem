#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef WIN32
#include <windows.h>
#endif
//-----------------------------------------------------------------------------
struct DateTimeStruct
{
	short Day;
	short Month;
	short Year;
	short Hour;
	short Minute;
	short Second;
	short Millisecond;
};
//-----------------------------------------------------------------------------
DateTimeStruct GetCurrentDateTime()
{
    DateTimeStruct DT;
#ifdef WIN32
	SYSTEMTIME ST;
	GetLocalTime(&ST);
	DT.Day = ST.wDay;
	DT.Month = ST.wMonth;
	DT.Year = ST.wYear;
	DT.Hour = ST.wHour;
	DT.Minute = ST.wMinute;
	DT.Second = ST.wSecond;
	DT.Millisecond = ST.wMilliseconds;
#else
    struct timeval TV;
    gettimeofday(&TV, NULL);
    int Millisecond = lrint(TV.tv_usec / 1000.0);
    if (Millisecond >= 1000)
    {
        Millisecond -= 1000;
        ++TV.tv_sec;
    }
    struct tm *TM = localtime(&TV.tv_sec);
    DT.Day = TM->tm_mday;
    DT.Month = TM->tm_mon + 1;
    DT.Year = TM->tm_year + 1900;
    DT.Hour = TM->tm_hour;
    DT.Minute = TM->tm_min;
    DT.Second = TM->tm_sec;
    DT.Millisecond = Millisecond;
#endif
    return DT;
}
//-----------------------------------------------------------------------------
int main(void)
{
    DateTimeStruct DT = GetCurrentDateTime();
    return 0;
}
//-----------------------------------------------------------------------------
