#include "ISDebug.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
void ShowDebugString(const char *Severity, const QString &String)
{
	//Получаем текущее время
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	//Формируем сообщение
	char Buffer[LOGGER_MESSAGE_SIZE];
	sprintf(Buffer, "%02d.%02d.%02d %02d:%02d:%02d:%03d [%s]\t%s",
		SystemTime.wDay, SystemTime.wMonth, SystemTime.wYear, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds,
		Severity, String.toStdString().c_str());
	std::cout << Buffer << std::endl;
}
//-----------------------------------------------------------------------------
void ShowDebugString(const QString &String)
{
	std::cout << String.toStdString() << std::endl;
}
//-----------------------------------------------------------------------------
