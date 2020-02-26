#include "ISDebug.h"
#include "ISConstants.h"
#include "ISProperty.h"
#include "ISLogger.h"
#include "ISSystem.h"
#include <sstream>
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
	if (MessageType == ISNamespace::DMT_Unknown) //Вывод неформатированной строки
	{
		printf("%s\n", Message.toStdString().c_str());
	}
	else
	{
		//Получаем текущую дату и время
		SYSTEMTIME ST;
		GetLocalTime(&ST);

		//Формируем начало строки лога (дата, время, идентификатор текущего потока)
		char Temp[MAX_PATH], YearString[5];
		itoa(ST.wYear, YearString, 10); //Преобразование года в строку
		sprintf(Temp, "%02d.%02d.%c%c %02d:%02d:%02d.%03d %d", ST.wDay, ST.wMonth, YearString[2], YearString[3], ST.wHour, ST.wMinute, ST.wSecond, ST.wMilliseconds, GetCurrentThreadId());
		// printf()?

		std::stringstream Stream;
		Stream << Temp;

		switch (MessageType)
		{
		case ISNamespace::DMT_Unknown:
		}
	}

	//QString NewString = String.simplified();
	//std::cout << NewString.toStdString() << std::endl;
	/*if (ISSystem::GetApplicationType() == ISNamespace::AT_CONSOLE)
	{
		QTextStream TextStream(stdout);

        if (ISSystem::GetCurrentOSType() == ISNamespace::OST_Windows)
        {
            TextStream.setCodec(TEXT_CODEC_IBM866);
        }

        TextStream << NewString << endl;
	}
	else if (ISSystem::GetApplicationType() == ISNamespace::AT_GUI)
	{
		qDebug().noquote() << NewString;
	}*/

	if (AddInLog)
	{
		//IS_LOGGER(NewString);
	}
}
//-----------------------------------------------------------------------------
