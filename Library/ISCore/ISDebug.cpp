#include "ISDebug.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
void ShowDebugString(const char *Severity, const QString &String)
{
    //Получаем текущее время
    unsigned int Day = 0, Month = 0, Year = 0, Hour = 0, Minute = 0, Second = 0, Millisecond = 0;
#ifdef WIN32
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
    Day = SystemTime.wDay;
    Month = SystemTime.wMonth;
    Year = SystemTime.wYear;
    Hour = SystemTime.wHour;
    Minute = SystemTime.wMinute;
    Second = SystemTime.wSecond;
    Millisecond = SystemTime.wMilliseconds;
#else
    struct timeval TimeValue;
    gettimeofday(&TimeValue, NULL);

    struct tm *SystemTime = localtime(&TimeValue.tv_sec);
    Day = SystemTime->tm_mday;
    Month = SystemTime->tm_mon + 1;
    Year = SystemTime->tm_year + 1900;
    Hour = SystemTime->tm_hour;
    Minute = SystemTime->tm_min;
    Second = SystemTime->tm_sec;
    Millisecond = (unsigned int)(TimeValue.tv_usec / 1000);
#endif
	//Формируем сообщение
	char Buffer[LOGGER_MESSAGE_SIZE] = { 0 };
	sprintf(Buffer, "%02d.%02d.%02d %02d:%02d:%02d:%03d [%s]\t%s",
        Day, Month, Year % 100, Hour, Minute, Second, Millisecond,
		Severity, String.toStdString().c_str());
	std::cout << Buffer << std::endl;
}
//-----------------------------------------------------------------------------
void ShowDebugString(const QString &String)
{
	std::cout << String.toStdString() << std::endl;
}
//-----------------------------------------------------------------------------
