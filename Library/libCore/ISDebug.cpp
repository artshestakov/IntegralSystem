#include "ISDebug.h"
#include "ISLogger.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
ISDebug::ISDebug()
{
	
}
//-----------------------------------------------------------------------------
ISDebug::~ISDebug()
{

}
//-----------------------------------------------------------------------------
ISDebug& ISDebug::GetInstance()
{
	static ISDebug Debug;
	return Debug;
}
//-----------------------------------------------------------------------------
void ISDebug::ShowString(const QString &Message)
{
	GetInstance().OutputString(ISNamespace::DMT_Unknown, Message);
}
//-----------------------------------------------------------------------------
void ISDebug::ShowDebugString(const QString &Message)
{
	GetInstance().OutputString(ISNamespace::DMT_Debug, Message);
}
//-----------------------------------------------------------------------------
void ISDebug::ShowInfoString(const QString &Message)
{
	GetInstance().OutputString(ISNamespace::DMT_Info, Message);
}
//-----------------------------------------------------------------------------
void ISDebug::ShowWarningString(const QString &Message)
{
	GetInstance().OutputString(ISNamespace::DMT_Warning, Message);
}
//-----------------------------------------------------------------------------
void ISDebug::ShowErrorString(const QString &Message)
{
	GetInstance().OutputString(ISNamespace::DMT_Error, Message);
}
//-----------------------------------------------------------------------------
void ISDebug::OutputString(ISNamespace::DebugMessageType MessageType, const QString &Message, bool AddInLog)
{
	bool b = DEFINES_CORE.IS_GUI;
	if (MessageType == ISNamespace::DMT_Unknown) //Вывод неформатированной строки
	{
		printf("%s\n", Message.toStdString().c_str());
	}
	else //Вывод форматированной строки
	{
		//Получаем текущую дату и время
		SYSTEMTIME ST;
		GetLocalTime(&ST);

		char Temp[MAX_PATH], YearString[5];
		itoa(ST.wYear, YearString, 10); //Преобразование года в строку

		std::string MessageTypeString;
		switch (MessageType)
		{
		case ISNamespace::DMT_Debug: MessageTypeString = "Info"; break;
		case ISNamespace::DMT_Info: MessageTypeString = "Debug"; break;
		case ISNamespace::DMT_Warning: MessageTypeString = "Warning"; break;
		case ISNamespace::DMT_Error: MessageTypeString = "Error"; break;
		}

		printf("%02d.%02d.%c%c %02d:%02d:%02d.%03d %d [%s] %s\n", ST.wDay, ST.wMonth, YearString[2], YearString[3], ST.wHour, ST.wMinute, ST.wSecond, ST.wMilliseconds, GetCurrentThreadId(), MessageTypeString.c_str(), Message.toStdString().c_str());
	}

	if (AddInLog)
	{
		//IS_LOGGER(NewString);
	}
}
//-----------------------------------------------------------------------------
