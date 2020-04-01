#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
//-----------------------------------------------------------------------------
struct DateTimeStruct
{
    DateTimeStruct() : Day(-1), Month(-1), Year(-1), Hour(-1), Minute(-1), Second(-1), Millisecond(-1) { }

    short Day, Month, Year, Hour, Minute, Second, Millisecond;
};
//-----------------------------------------------------------------------------
DateTimeStruct GetCurrentDateTime()
{
    DateTimeStruct DT;
#ifdef WIN32
    // Windows
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
int main(/*int argc, char **argv*/void)
{
    DateTimeStruct DT = GetCurrentDateTime();
    return 0;
}
//-----------------------------------------------------------------------------
